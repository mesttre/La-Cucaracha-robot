//motor A
const byte MotorA_Pos = 22 ; // Motor A Input A --> MOTOR A (+)
const byte MotorA_Neg = 23 ; // Motor A Input B --> MOTOR A (-) (PWM)

//motor B
const byte MotorB_Pos = 21; // Motor B Input A --> MOTOR B (+)
const byte MotorB_Neg = 19; // Motor B Input B --> MOTOR B (-) (PWM)

//velocidade max
const byte VEL_MAX = 255;

int motorChannelAP = 1;
int motorChannelAN = 2;
int motorChannelBP = 3;
int motorChannelBN = 4;

 // o numero do pino do bot�o
const int buttonPin = 18;
const int sensorPin = 17;
const int aspiradoPin = 16;
const int vassouraPin = 15;
bool sentido = true;         // variavel para ler o estado do botao

int RGS0 = 0x00;
int SB3 = 0x00;
int AA4 = 0x00;
int AV5 = 0x00;
int RE6 = 0x00;
int REFT7 = 0x00;
int RD8 = 0x00;
int RDFT9 = 0x00;

//Inicializa Pinos
void setup(){
Serial.begin(9600);

 // configura��o dos pinos como sa�da do esp32
 
 
 pinMode(MotorA_Pos,OUTPUT); // motor A
 pinMode(MotorA_Neg,OUTPUT); // motor A
 pinMode(MotorB_Pos,OUTPUT); // motor B
 pinMode(MotorB_Neg,OUTPUT); // motor B

  ledcSetup(motorChannelAP, 5000, 8);
  ledcSetup(motorChannelAN, 5000, 8);
  ledcSetup(motorChannelBP, 5000, 8);
  ledcSetup(motorChannelBN, 5000, 8);
  ledcAttachPin(MotorA_Pos, motorChannelAP);
  ledcAttachPin(MotorA_Neg, motorChannelAN);
  ledcAttachPin(MotorB_Pos, motorChannelBP);
  ledcAttachPin(MotorB_Neg, motorChannelBN);

 
 pinMode(buttonPin, INPUT_PULLUP);
 pinMode(sensorPin, INPUT);
 pinMode(aspiradoPin, OUTPUT);
 pinMode(vassouraPin, OUTPUT);

 // inicia com os motores desligados
 digitalWrite(MotorA_Pos,LOW);
 digitalWrite(MotorA_Neg,LOW);
 digitalWrite(MotorB_Pos, LOW);
 digitalWrite(MotorB_Neg,LOW);
 Serial.println("Preparando a La Cucaracha");
 paraVassoura ();
 Serial.println("pronto");
}

void para (){
Serial.println("para");
digitalWrite(MotorA_Pos,LOW);
digitalWrite(MotorA_Neg,LOW);
digitalWrite(MotorB_Pos, LOW);
digitalWrite(MotorB_Neg,LOW);
}

void iniciaAspirador (){
Serial.println("iniciaAspirador");
digitalWrite(aspiradoPin,HIGH);
}
void paraAspirador (){
Serial.println("paraAspirador");
digitalWrite(aspiradoPin,LOW);
}

void iniciaVassoura (){
    Serial.println("iniciaVassoura");
    digitalWrite(vassouraPin,HIGH);
    AV5 = 0x01;
}
void paraVassoura (){
    Serial.println("paraVassoura");
    digitalWrite(vassouraPin,LOW);
    AV5 = 0x00;
}

void vaiPraFrente (){
Serial.println("vaiPraFre");
 digitalWrite(MotorA_Pos,HIGH);
 digitalWrite(MotorA_Neg,LOW);

 digitalWrite(MotorB_Pos, HIGH);
 digitalWrite(MotorB_Neg, LOW);
}

void vaiPraTras(){
  Serial.println("vaiPraTras");
 digitalWrite(MotorA_Pos,LOW);
 digitalWrite(MotorA_Neg,HIGH);

 digitalWrite(MotorB_Pos, LOW);
 digitalWrite(MotorB_Neg, HIGH);
}
void vaiPraDireita(){
 Serial.println("vaiPraDireita");
 digitalWrite(MotorA_Pos,HIGH);
 digitalWrite(MotorA_Neg,LOW);

 digitalWrite(MotorB_Pos, LOW);
 digitalWrite(MotorB_Neg, HIGH);
}

void vaiPraEsquerda(){
 Serial.println("vaiPraEsquerda");
 digitalWrite(MotorA_Pos, LOW);
 digitalWrite(MotorA_Neg, HIGH);

 digitalWrite(MotorB_Pos,HIGH);
 digitalWrite(MotorB_Neg,LOW);

}

void getStatus(){
  char buffer[100];
  sprintf(buffer, "RES GET_STATUS [ %d , %d , %d , %d , %d , %d , %d , %d , %d , %d ]\n", RGS0,0,0,SB3,AA4,AV5,RE6,REFT7,RD8,RDFT9);
  Serial.println(buffer);
}


void loop() {

//  Serial.println("[0x00,0x00, 0x00, 0x00, 0x00, **AV**, 0x00, 0x00, 0x00, 0x00]");
      String serialCommand;

    if (Serial.available() > 0) {
        serialCommand = Serial.readStringUntil('\n');
        processCommand(serialCommand);
        getStatus();
    }
	  if (digitalRead(sensorPin) == HIGH) {
		  Serial.println("Parede");
      SB3 = 1;
      getStatus();
      delay(200);
	  }else{
      SB3 = 0;
	  }
  if (digitalRead(buttonPin) == LOW) {
	  Serial.println("Botao");
	  delay(200);
  }
  
}

void processCommand(String command) {
    command.trim();
    command.toUpperCase();
//     Serial.println("RES "+ command + " DBG Received command: " + command+"<<<");
     if (command == "GET_STATUS"){
        getStatus();
     }
     else if (command.startsWith("SET_ASP")){
          String tmp = command.substring(7);
          tmp.trim();
          if(tmp=="L"){
            iniciaAspirador();
            AA4 = 1;
          }
          if(tmp=="D"){
            paraAspirador();
            AA4 = 0;
          }
     }
     else if (command.startsWith("SET_VAS")){
          String tmp = command.substring(7);
          tmp.trim();
          if(tmp=="L"){
            iniciaVassoura();
            AV5 = 1;
          }
          if(tmp=="D"){
            paraVassoura();
            AV5 = 0;
          }
     }
    
     else if (command.startsWith("SET_RODA")){
          String tmpPower = command.substring(9,10);
          tmpPower.trim();
          String tmpDir = command.substring(11,12);
          tmpDir.trim();
          String tmpVel = command.substring(12);
          tmpVel.trim();
          int tmpVelInt = tmpVel.toInt();
          if(tmpPower=="E" && tmpDir=="T"){//min 170 max 256
            ledcWrite(motorChannelAP, 0);
            ledcWrite(motorChannelAN, tmpVelInt);
            RE6 = tmpVelInt;
            REFT7 = 1; 
          }
          else if(tmpPower=="E" && tmpDir=="F"){//min 170 max 256
            ledcWrite(motorChannelAP, tmpVelInt);
            ledcWrite(motorChannelAN, 0);
            RE6 = tmpVelInt;
            REFT7 = 0; 
          }
          else if(tmpPower=="D" && tmpDir=="T"){//min 170 max 256
             Serial.println("LF");
            ledcWrite(motorChannelBP, 0);
            ledcWrite(motorChannelBN, tmpVelInt);
            RD8 = tmpVelInt;
            RDFT9 = 1; 
          }
          else if(tmpPower=="D" && tmpDir=="F"){//min 170 max 256
            ledcWrite(motorChannelBP, tmpVelInt);
            ledcWrite(motorChannelBN, 0);
            RD8 = tmpVelInt;
            RDFT9 = 0; 
          }
          else if(tmpPower=="T" && tmpDir=="T"){//min 170 max 256
            ledcWrite(motorChannelAP, 0);
            ledcWrite(motorChannelAN, tmpVelInt);
            ledcWrite(motorChannelBP, 0);
            ledcWrite(motorChannelBN, tmpVelInt);
            RE6 = tmpVelInt;
            REFT7 = 1; 
            RD8 = tmpVelInt;
            RDFT9 = 1; 
          }
          else if(tmpPower=="T" && tmpDir=="F"){//min 170 max 256
            ledcWrite(motorChannelAP, tmpVelInt);
            ledcWrite(motorChannelAN, 0);
            ledcWrite(motorChannelBP, tmpVelInt);
            ledcWrite(motorChannelBN, 0);
            RE6 = tmpVelInt;
            REFT7 = 0; 
            RD8 = tmpVelInt;
            RDFT9 = 0; 
          }
          
     }
}
