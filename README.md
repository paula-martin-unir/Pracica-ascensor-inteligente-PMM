# Practica-ascensor-inteligente-PMM
# 🛗 Proyecto de Ascensor Inteligente - ACME S.A.

Este proyecto consiste en el desarrollo de un sistema de control embebido para un ascensor de 5 plantas, integrando sensores y actuadores bajo una lógica de eficiencia energética.

## 🚀 Enlace a la Simulación
[Haz clic aquí para ver el proyecto en WOKWI]https://wokwi.com/projects/463353049165237249

## 🛠️ Componentes Utilizados
- **Arduino Uno** como unidad de control.
- **Servomotor** para el movimiento de la cabina (Plantas G a 4).
- **Sensor DHT22** para control de temperatura (Lógica de Zona Muerta).
- **Sensor LDR** para iluminación progresiva de cabina.
- **Pantalla LCD I2C** como interfaz HMI.

## 🧠 Lógica de Control
- **Climatización:** Algoritmo de 3 posiciones con zona muerta (+/- 3°C sobre el setpoint de 25°C).
- **Iluminación:** Control escalonado de 5 LEDs según la luz ambiental detectada.
- **Movimiento:** Gestión de llamadas mediante pulsadores con indicación de dirección (Subiendo/Bajando).
