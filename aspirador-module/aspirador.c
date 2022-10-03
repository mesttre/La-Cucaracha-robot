#include <linux/module.h>
#include <linux/usb.h>
#include <linux/slab.h>

MODULE_AUTHOR("DevTITANS <devtitans@icomp.ufam.edu.br>");
MODULE_DESCRIPTION("Driver de acesso ao Aspirador de po (ESP32 com Chip Serial CP2102");
MODULE_LICENSE("GPL");

// Tamanho máximo de uma linha de resposta do dispositvo USB
#define MAX_RECV_LINE 100

static int  usb_probe(struct usb_interface *ifce, const struct usb_device_id *id); // Executado quando o dispositivo é conectado na USB
static void usb_disconnect(struct usb_interface *ifce);                            // Executado quando o dispositivo USB é desconectado da USB                                 // Envia um comando via USB e espera/retorna a resposta do dispositivo (int)
// Executado quando o arquivo /sys/kernel/aspirador/{status, roda, vas, asp} é lido (e.g., cat /sys/kernel/aspirador/status)
static ssize_t attr_show(struct kobject *sys_obj, struct kobj_attribute *attr, char *buff);
// Executado quando o arquivo /sys/kernel/aspirador/{status, roda, vas, asp} é escrito (e.g., echo "100" | sudo tee -a /sys/kernel/aspirador/status)
static ssize_t attr_store(struct kobject *sys_obj, struct kobj_attribute *attr, const char *buff, size_t count);

static char recv_line[MAX_RECV_LINE];              // Armazena dados vindos da USB até receber um caractere de nova linha '\n'
static struct usb_device *aspirador_device;//aspirador_driver;        // Referência para o dispositivo USB
static uint usb_in, usb_out;                       // Endereços das portas de entrada e saida da USB
static char *usb_in_buffer, *usb_out_buffer;       // Buffers de entrada e saída da USB
static int usb_max_size;                           // Tamanho máximo de uma mensagem USB

// Variáveis para criar os arquivos no /sys/kernel/aspirador/{status, roda, vas, asp}
static struct kobj_attribute  status_attribute = __ATTR(status, S_IRUGO | S_IWUSR, attr_show, attr_store);
static struct kobj_attribute  roda_attribute = __ATTR(roda, S_IRUGO | S_IWUSR, attr_show, attr_store);
static struct kobj_attribute  vas_attribute = __ATTR(vas, S_IRUGO | S_IWUSR, attr_show, attr_store);
static struct kobj_attribute  asp_attribute = __ATTR(asp, S_IRUGO | S_IWUSR, attr_show, attr_store);

static struct attribute      *attrs[]       = { &status_attribute.attr, &roda_attribute.attr, &vas_attribute.attr, &asp_attribute.attr, NULL };
static struct attribute_group attr_group    = { .attrs = attrs };
static struct kobject        *sys_obj;

// Registra o CP2102 (Chip USB-Serial do ESP32) no USB-Core do kernel
#define VENDOR_ID  0x10C4  /* VendorID  do CP2102 */
#define PRODUCT_ID 0xEA60  /* ProductID do CP2102 */

//#define VENDOR_ID  0x1A86  /* VendorID  do CP2102 */
//#define PRODUCT_ID 0x55D4  /* ProductID do CP2102 */
static const struct usb_device_id id_table[] = { { USB_DEVICE(VENDOR_ID, PRODUCT_ID) }, {} };
MODULE_DEVICE_TABLE(usb, id_table);

// Cria e registra o driver do aspirador no kernel
static struct usb_driver aspirador_driver = {
    .name        = "aspirador",     // Nome do driver
    .probe       = usb_probe,       // Executado quando o dispositivo é conectado na USB
    .disconnect  = usb_disconnect,  // Executado quando o dispositivo é desconectado na USB
    .id_table    = id_table,        // Tabela com o VendorID e ProductID do dispositivo
};
module_usb_driver(aspirador_driver);

// Executado quando o dispositivo é conectado na USB
static int usb_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    struct usb_endpoint_descriptor *usb_endpoint_in, *usb_endpoint_out;

    printk(KERN_INFO "1 Aspirador de po: Dispositivo conectado ...\n");

    // Cria arquivos do /sys/kernel/aspirador/*
    sys_obj = kobject_create_and_add("aspirador", kernel_kobj);
    sysfs_create_group(sys_obj, &attr_group);

    // Detecta portas e aloca buffers de entrada e saída de dados na USB
    aspirador_device = interface_to_usbdev(interface);
    usb_find_common_endpoints(interface->cur_altsetting, &usb_endpoint_in, &usb_endpoint_out, NULL, NULL);
    usb_max_size = usb_endpoint_maxp(usb_endpoint_in);
    usb_in = usb_endpoint_in->bEndpointAddress;
    usb_out = usb_endpoint_out->bEndpointAddress;
    usb_in_buffer = kmalloc(usb_max_size, GFP_KERNEL);
    usb_out_buffer = kmalloc(usb_max_size, GFP_KERNEL);

    return 0;
}

// Executado quando o dispositivo USB é desconectado da USB
static void usb_disconnect(struct usb_interface *interface) {
    printk(KERN_INFO "2 Aspirador de po: Dispositivo desconectado.\n");
    if (sys_obj) kobject_put(sys_obj);      // Remove os arquivos em /sys/kernel/aspirador
    kfree(usb_in_buffer);                   // Desaloca buffers
    kfree(usb_out_buffer);
}

// Enviar comando USB com o parametro string
static int usb_send_cmd_str(char *cmd, char *param, char* szBufOut) {
    int recv_size = 0;                      // Quantidade de caracteres no recv_line
    int ret, actual_size, i;
    int retries = 10;                       // Tenta algumas vezes receber uma resposta da USB. Depois desiste.
    char resp_expected[MAX_RECV_LINE];      // Resposta esperada do comando
    char *resp_pos;                         // Posição na linha lida que contém o número retornado pelo dispositivo
    long resp_number = -1;                  // Número retornado pelo dispositivo (e.g., valor do led, valor do ldr)

    printk(KERN_INFO "3 Aspirador de po: Enviando comando: %s\n", cmd);

    if ( ( NULL != param ) && ( strlen( param ) >= 0 ) ){
        printk(KERN_INFO "3.1 Aspirador de po: Enviando comando: %s | %s\n", cmd, param);
        sprintf(usb_out_buffer, "%s %s\n", cmd, param); // Se param >=0, o comando possui um parâmetro (int)
    }
    else sprintf(usb_out_buffer, "%s\n", cmd);                      // Caso contrário, é só o comando mesmo

    // Envia o comando (usb_out_buffer) para a USB

    ret = usb_bulk_msg(aspirador_device, usb_rcvbulkpipe(aspirador_device, usb_in), usb_in_buffer, min(usb_max_size, MAX_RECV_LINE), &actual_size, 500);
//    usb_clear_halt(aspirador_device, usb_rcvbulkpipe(aspirador_device, usb_in) );

    ret = usb_bulk_msg(aspirador_device, usb_sndbulkpipe(aspirador_device, usb_out), usb_out_buffer, strlen(usb_out_buffer), &actual_size, 1000*HZ);
    if (ret) {
        printk(KERN_ERR "4 Aspirador de po: Erro de codigo %d ao enviar comando!\n", ret);
        return -1;
    }

//     sprintf(resp_expected, "RES %s", cmd);  // Resposta esperada. Ficará lendo linhas até receber essa resposta.
    sprintf(resp_expected, "RES GET_STATUS");  // Resposta esperada. Ficará lendo linhas até receber essa resposta.
    printk(KERN_INFO "4.1 Tst\n");

    // Espera pela resposta correta do dispositivo (desiste depois de várias tentativas)
    while (retries > 0) {
        // Lê dados da USB
        ret = usb_bulk_msg(aspirador_device, usb_rcvbulkpipe(aspirador_device, usb_in), usb_in_buffer, min(usb_max_size, MAX_RECV_LINE), &actual_size, HZ*1000);
        if (ret) {
            printk(KERN_ERR "5 Aspirador de po: Erro ao ler dados da USB (tentativa %d). Codigo: %d\n", ret, retries--);
            continue;
        }
        printk(KERN_INFO "4.2 Tst\n");
        // Para cada caractere recebido ...
        for (i=0; i<actual_size; i++) {
            printk(KERN_INFO "4.3 Tst\n");
            if (usb_in_buffer[i] == '\n') {  // Temos uma linha completa
                recv_line[recv_size] = '\0';
                printk(KERN_INFO "6 Aspirador de po: Recebido uma linha: '%s'\n", recv_line);

                // Verifica se o início da linha recebida é igual à resposta esperada do comando enviado
                if (!strncmp(recv_line, resp_expected, strlen(resp_expected))) {
                    printk(KERN_INFO "7 Aspirador de po: Linha eh resposta para %s! Processando ...\n", cmd);

                    // Acessa a parte da resposta que contém o número e converte para inteiro
//                     resp_pos = &recv_line[strlen(resp_expected) + 1];
//                     kstrtol(resp_pos, 10, &resp_number);
                    strncpy(szBufOut, recv_line, recv_size);

                    return 1;
                }
                else { // Não é a linha que estávamos esperando. Pega a próxima.
                    printk(KERN_INFO "8 Aspirador de po: Nao eh resposta para %s! Tentiva %d. Proxima linha...\n", cmd, retries--);
                    recv_size = 0; // Limpa a linha lida (recv_line)
                }
            }
            else { // É um caractere normal (sem ser nova linha), coloca no recv_line e lê o próximo caractere
                recv_line[recv_size] = usb_in_buffer[i];
                recv_size++;
            }
        }
        printk(KERN_INFO "4.4 Tst\n");
    }
    return -1; // Não recebi a resposta esperada do dispositivo
}


// Executado quando o arquivo /sys/kernel/aspirador/{status, roda, vas, asp} é lido (e.g., cat /sys/kernel/smartlamp/status)
static ssize_t attr_show(struct kobject *sys_obj, struct kobj_attribute *attr, char *buff) {
    int value = -1;
    const char *attr_name = attr->attr.name;
    char szBuffer[128];

    printk(KERN_INFO "9 Aspirador de po: Lendo %s ...\n", attr_name);

    memset(szBuffer, 0, sizeof(szBuffer));

    if (!strcmp(attr_name, "status"))
        value = usb_send_cmd_str("GET_STATUS", " ", szBuffer);
/*
    else if (!strcmp(attr_name, "ldr"))
        value = usb_send_cmd("GET_LDR", -1);
    else
        value = usb_send_cmd("GET_THRESHOLD", -1);
*/
    printk(KERN_INFO "10 Aspirador de po: retorno %d, [%s] ...\n", value, szBuffer);


    sprintf(buff, "%s\n", szBuffer);                   // Cria a mensagem com o valor do led, ldr ou threshold

    return strlen(buff);
}

// Executado quando o arquivo /sys/kernel/aspirador/{status, roda, vas, asp} é escrito (e.g., echo "100" | sudo tee -a /sys/kernel/smartlamp/led)
static ssize_t attr_store(struct kobject *sys_obj, struct kobj_attribute *attr, const char *buff, size_t count) {
    long ret = -1;
    //long value = -1;
    char szValue[128];
    char szBufferOut[128];
    const char *attr_name = attr->attr.name;

    memset(szValue, 0, sizeof(szValue) );

    //ret = kstrtol(buff, 10, &value);
    strncpy(szValue, buff,count-1);
        printk(KERN_ALERT "111 Aspirador de po: valor de %s buffer %s.\n", attr_name, buff);
    //if (ret) {
      //  printk(KERN_ALERT "11 Aspirador de po: valor de %s invalido %s.\n", attr_name, buff);
        //return -EACCES;
    //}

    //printk(KERN_INFO "12 Aspirador de po: Setando %s para %ld ...\n", attr_name, value);
    printk(KERN_INFO "12 Aspirador de po: Setando %s para %s ...\n", attr_name, szValue);

    if (!strcmp(attr_name, "roda")) {
        //ret = usb_send_cmd("SET_RODA", value);
        ret = usb_send_cmd_str("SET_RODA", szValue, szBufferOut);
    }
    else if (!strcmp(attr_name, "asp"))
        //ret = usb_send_cmd("SET_ASP", value);
        ret = usb_send_cmd_str("SET_ASP", szValue, szBufferOut);
    else if (!strcmp(attr_name, "vas"))
        //ret = usb_send_cmd("SET_VAS", value);
        ret = usb_send_cmd_str("SET_VAS", szValue, szBufferOut);
    else {
        printk(KERN_ALERT "13 Aspirador de po: o valor do comando esta errado.\n");
        return -EACCES;
    }

            printk(KERN_ALERT "14 Aspirador de po: retorno do comando ret=%d.\n", ret);

    if (ret < 0) {
        printk(KERN_ALERT "15 Aspirador de po: erro ao setar o valor do %s.\n", attr_name);
        return -EACCES;
    }

    return strlen(buff);
}
