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
int AV5 = 0x00;

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




void loop() {

//  Serial.println("[0x00,0x00, 0x00, 0x00, 0x00, **AV**, 0x00, 0x00, 0x00, 0x00]");
      String serialCommand;

    if (Serial.available() > 0) {
        serialCommand = Serial.readStringUntil('\n');
        processCommand(serialCommand);
    }
//	  if (Serial.available() > 0) {
//	    // l� do buffer o dado recebido:
//	    char incomingByte = Serial.read();
//
//	    // responde com o dado recebido:
//	    Serial.print("I received: ");
//	    Serial.println(incomingByte, DEC);
//	    switch(incomingByte){
//	    case 'a':
//	    	vaiPraEsquerda();
//	    	break;
//	    case 's':
//	    	vaiPraTras ();
//	    	break;
//	    case 'd':
//	    	vaiPraDireita();
//	    	break;
//	    case 'w':
//	    	vaiPraFrente ();
//	    	break;
//	    case 'q':
//	    	para();
//	    	break;
//      case 'e':
//        iniciaAspirador();
//        break;
//      case 'r':
//        paraAspirador();
//        break;
//      case 'f':
//        iniciaVassoura();
//        break;
//      case 'g':
//        paraVassoura();
//        break;
//     }
//	  }
	  if (digitalRead(sensorPin) == HIGH) {
		  Serial.println("sensor");
	  }
  if (digitalRead(buttonPin) == LOW) {
	  Serial.println("parede");
	  delay(200);
  }
}

void processCommand(String command) {
    command.trim();
    command.toUpperCase();
     Serial.println("DBG Received command: " + command+"<<<");
     if (command == "GET_STATUS"){
          char buffer[50];
          sprintf(buffer, "[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]", RGS0,1,2,3,4,AV5,6,7,8,9);
          Serial.println(buffer);
     }
     else if (command.startsWith("SET_VAS")){
          String tmp = command.substring(7);
          tmp.trim();
          if(tmp=="L") iniciaVassoura();
          if(tmp=="D") paraVassoura();
          Serial.println(">>"+tmp+"<<");
     }

          else if (command.startsWith("SET_VAS")){
          String tmp = command.substring(7);
          tmp.trim();
          if(tmp=="L") iniciaVassoura();
          if(tmp=="D") paraVassoura();
          Serial.println(">>"+tmp+"<<");
     }
     else if (command.startsWith("SET_RODA")){
          String tmpPower = command.substring(9,10);
          tmpPower.trim();
          String tmpDir = command.substring(11,12);
          tmpDir.trim();
          String tmpVel = command.substring(12);
          tmpVel.trim();
          int tmpVelInt = tmpVel.toInt();
          Serial.println(">>"+tmpPower+"<<");
          Serial.println(">>"+tmpDir+"<<");
          Serial.println(">>"+String(tmpVelInt)+"<<");
          if(tmpPower=="E" && tmpDir=="T"){//min 170 max 256
            ledcWrite(motorChannelAP, 0);
            ledcWrite(motorChannelAN, tmpVelInt);
          }
          else if(tmpPower=="E" && tmpDir=="F"){//min 170 max 256
            ledcWrite(motorChannelAP, tmpVelInt);
            ledcWrite(motorChannelAN, 0);
          }
          if(tmpPower=="D" && tmpDir=="T"){//min 170 max 256
             Serial.println("LF");
            ledcWrite(motorChannelBP, 0);
            ledcWrite(motorChannelBN, tmpVelInt);
          }
          else if(tmpPower=="D" && tmpDir=="F"){//min 170 max 256
            ledcWrite(motorChannelBP, tmpVelInt);
            ledcWrite(motorChannelBN, 0);
          }
          
     }
}
