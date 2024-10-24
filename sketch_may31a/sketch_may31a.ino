#include <Servo.h>

Servo myservo;  // 서보 객체 생성
bool keepRotating = false;  // 회전 상태를 저장하는 변수

void setup() {
  myservo.attach(9);  // 서보를 아두이노의 9번 핀에 연결
  Serial.begin(9600); // 시리얼 통신 시작
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read(); // 시리얼 포트에서 입력 읽기
    
    if (command == '1') {
      keepRotating = true;  // 회전 상태를 true로 설정
    } else if (command == '2') {
      keepRotating = false;  // 회전 상태를 false로 설정
      myservo.write(90);     // 모터 정지 (중립 위치)
    }
  }
  
  if (keepRotating) {
    myservo.write(180); // 시계 방향으로 최대 회전
    delay(700);        // 회전 시간 조정 (이 값은 서보 모터의 속도에 따라 조정해야 합니다)
    myservo.write(0);   // 반시계 방향으로 최대 회전
    delay(700);        // 회전 시간 조정
  }
}
