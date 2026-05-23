# Artificial Pancreas Model - Closed-Loop Glucose Control System

## 🩺 Overview

This project implements a **hardware-in-the-loop model** of an artificial pancreas using an Arduino-compatible microcontroller. It demonstrates closed-loop glucose regulation by using distilled water (simulating high blood glucose) and tap water (simulating insulin) with conductivity sensing.

## 🧠 Real-Time Systems Concepts Demonstrated

| Concept | Implementation |
|---------|----------------|
| Periodic Task | Sensor reading every 500ms |
| Event-Driven Task | Pump activation on threshold crossing |
| State Machine | Glucose level classification (3 states) |
| Safety Interlock | Pump max duration = 3 seconds |
| Real-Time Constraints | Response time < 1 second |

## 🔧 Hardware Components

- Arduino-compatible microcontroller
- Peristaltic liquid pump (5V)
- N-channel MOSFET (switching)
- Conductivity sensor (voltage divider)
- Breadboard and jumper wires

## 📊 System Behavior
High Glucose (Distilled Water) → High Voltage → Pump ON → Insulin (Tap Water) Added
↓
Low Glucose (Tap Water mixed) ← Low Voltage ← Pump OFF ← Target Reached

## 📈 Future Improvements

- [ ] Implement PID control for smoother insulin delivery
- [ ] Add Bluetooth logging to mobile app
- [ ] Port to FreeRTOS with separate tasks for sensing/actuation/logging

## 📊 Experimental Results

### Raw Sensor Data

| Measurement | Before Insulin (High Glucose) | After Insulin |
|-------------|-------------------------------|---------------|
| Minimum | 694 | 389 |
| Maximum | 703 | 417 |
| Average | **698** | **400** |

### Interpretation

- **698 = High "blood glucose"** (distilled water, low conductivity)
- **400 = Reduced glucose** (tap water added, increased conductivity)
- **↓298 point drop** demonstrating successful closed-loop control

### Sample Readings
Initial: 701 → Pump ON
After pump: 398
Initial: 698 → Pump ON
After pump: 389
Initial: 694 → Pump ON
After pump: 395

Before average = (701+698+694+694+695+698+702+703+700) ÷ 9 = 698.3
After average = (398+389+395+417+395+408+396+411+394) ÷ 9 = 400.3
Drop = 298 points (≈42.7% reduction)

The system consistently responded by adding "insulin" (tap water) whenever glucose was high, successfully lowering the reading by ~300 points each time.

## 📝 License

MIT
