# 🐣Hudadak🐔
병아리에서 닭까지. IoT 후다닭 프로젝트

## 🐤프로젝트 목적
* 9~16세 아이들이 사용하기 쉽도록 간단한 시스템을 구현한다.
* 아이들의 감수성을 자극시키면서 안전한 시스템을 구축한다.
* 시스템의 모듈화를 통해 차후 큰 양계장에도 적용될 수 있는 정교한 시스템을 구축한다.
* 가정용 육수 시스템에서 데이터를 수집하여 양계장에도 적용할 수 있도록 한다.

## 🐤프로젝트 기능 및 성능
*	이 스마트 육추장은 임베디드 시스템 기반의 IoT서비스를 제공한다.
*	ESP32모듈을 기기로 사용하여 저전력 IoT어플리케이션을 운용한다.
*	온도센서를 부착하여 육추장 내부의 온도를 파악한다. 병아리의 육추 시기에 따라 자동으로 적외선 램프를 이용하여 내부의 온도를 맞춰준다. 

 <p align="center"> <img src="https://user-images.githubusercontent.com/35029025/141653014-bf7eaf14-6d87-43bf-b075-8fe6a9e958e3.png"></p>

 <p align="center"> 그림 1 육추시기에 따른 적정 온도표 </p>
 
*	마이크를 달아 항시 병아리의 소리를 수음한다. 병아리의 움직임은 활력 징후, 온도의 상태, 천적의 침입 여부를 가늠할 수 있는 힌트가 된다.

<p align="center"> <img src="https://user-images.githubusercontent.com/35029025/141653020-640f21e3-6745-42b2-8e7d-bf068f3beefd.png"></p>
<p align="center"> 그림 2 온도와 병아리의 상태</p>

* ESP32-CAM을 달아 병아리의 상태를 찍고, S3에 전송한다. 사용자는 어플리케이션에 접근하여 병아리들의 상태를 확인할 수 있다.

<p align="center"><img src="https://user-images.githubusercontent.com/35029025/146713121-fe7ab816-2dfb-4038-842a-ca2f679d3c1a.png" width="40%"></p>
<p align="center"> 그림 3 BROKER역할을 하는 ESP32가 제공하는 Application</p>


## 🐣후다닭 시나리오
<p align="center"> <img src="https://user-images.githubusercontent.com/35029025/144736813-a9304b20-9011-4fb4-84d6-915581051a85.png" width="70%"></p>

## 🐣후다닭 프레임워크
<p align="center"> <img src="https://user-images.githubusercontent.com/35029025/144736829-d519bea8-8645-40b9-9755-447bfd444357.png" width="70%"></p>




