/*
 * Artificial Pancreas Model - Closed-Loop Glucose Control System
 * 
 * Simulates an insulin pump responding to blood glucose levels.
 * Uses distilled water (high glucose) and tap water (insulin) to model conductivity.
 * 
 * RTOS Concepts Demonstrated:
 * - Periodic sensing (loop with delay = sampling period)
 * - Event-driven actuation (pump triggers on threshold crossing)
 * - Safety interlock (pump cannot exceed safe duty cycle)
 * - State machine for glucose level classification
 */

// Pin Definitions
const int PUMP_PIN = 11;          // PWM pin for peristaltic pump control
const int SENSOR_PIN = A0;        // Analog input for conductivity sensor
const int LED_PIN = 13;           // Built-in LED for system status

// Pump Parameters
const int PUMP_MAX_SPEED = 255;   // Maximum pump speed (0-255)
const int PUMP_MIN_SPEED = 0;     // Pump off
const unsigned long PUMP_SAFE_DURATION_MS = 3000;  // Max continuous pump run time (safety)

// Glucose (Conductivity) Thresholds
const int GLUCOSE_HIGH_THRESHOLD = 210;   // Above this = high glucose → need insulin
const int GLUCOSE_TARGET_MIN = 180;       // Target range lower bound (safe zone)
const int GLUCOSE_TARGET_MAX = 220;       // Target range upper bound (safe zone)

// Timing Parameters (Real-Time Control Loop)
const unsigned long SAMPLING_PERIOD_MS = 500;   // Read sensor every 500ms (2Hz)
const unsigned long ACTUATION_PERIOD_MS = 100;  // Pump control cycle every 100ms

// System State Variables
int currentGlucoseLevel = 0;        // Current sensor reading (0-1023)
bool pumpActive = false;             // Pump state
unsigned long pumpStartTime = 0;     // When pump was activated (for safety cutoff)

// Glucose Level Classification Enum
enum GlucoseState {
    STATE_HYPOGLYCEMIA,    // Too low (dangerous - pump MUST be off)
    STATE_TARGET,          // Normal range
    STATE_HYPERGLYCEMIA    // Too high (need insulin)
};

GlucoseState currentState = STATE_TARGET;

void setup() {
    // Initialize hardware
    pinMode(PUMP_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    
    // Ensure pump starts in OFF state (safety)
    digitalWrite(PUMP_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    
    // Initialize serial communication for debugging
    Serial.begin(9600);
    Serial.println("Artificial Pancreas System Initialized");
    Serial.println("---");
    delay(1000);
}

void loop() {
    // Task 1: Periodic Sensor Reading (Period = SAMPLING_PERIOD_MS)
    readGlucoseLevel();
    
    // Task 2: State Classification & Decision Logic
    classifyGlucoseState();
    
    // Task 3: Actuator Control (Insulin Pump)
    controlPump();
    
    // Task 4: Safety Monitoring
    checkSafetyOverrides();
    
    // Task 5: Logging & Status Update
    logSystemStatus();
    
    // Maintain precise sampling period (simple RTOS tick simulation)
    delay(SAMPLING_PERIOD_MS);
}

/*
 * Task 1: Read conductivity sensor
 * Input: Distilled water (high glucose) = low conductivity = high voltage reading
 *        Tap water (insulin) = high conductivity = low voltage reading
 */
void readGlucoseLevel() {
    currentGlucoseLevel = analogRead(SENSOR_PIN);
    
    // Apply simple moving average filter (reduces sensor noise)
    static int filteredValues[5] = {0};
    static int index = 0;
    static int sum = 0;
    
    sum -= filteredValues[index];
    filteredValues[index] = currentGlucoseLevel;
    sum += filteredValues[index];
    index = (index + 1) % 5;
    
    currentGlucoseLevel = sum / 5;  // 5-point average
}

/*
 * Task 2: Classify current glucose level into medical states
 * Models the biological decision process of the pancreas
 */
void classifyGlucoseState() {
    if (currentGlucoseLevel < GLUCOSE_TARGET_MIN) {
        currentState = STATE_HYPOGLYCEMIA;
    } 
    else if (currentGlucoseLevel > GLUCOSE_TARGET_MAX) {
        currentState = STATE_HYPERGLYCEMIA;
    } 
    else {
        currentState = STATE_TARGET;
    }
}

/*
 * Task 3: Pump control logic (Actuator)
 * HIGH glucose → need insulin → pump ON
 * TARGET glucose → maintain
 * LOW glucose → pump OFF (safety - prevent hypoglycemia)
 */
void controlPump() {
    switch(currentState) {
        case STATE_HYPERGLYCEMIA:
            // Blood glucose too high - administer insulin
            if (!pumpActive) {
                pumpActive = true;
                pumpStartTime = millis();
                analogWrite(PUMP_PIN, PUMP_MAX_SPEED);
                digitalWrite(LED_PIN, HIGH);
                Serial.println("PUMP: ON (Hyperglycemia detected)");
            }
            break;
            
        case STATE_TARGET:
            // Blood glucose in target range - maintain (no action)
            if (pumpActive) {
                stopPump();
                Serial.println("PUMP: OFF (Target range reached)");
            }
            break;
            
        case STATE_HYPOGLYCEMIA:
            // CRITICAL: Blood glucose too low - MUST stop insulin
            if (pumpActive) {
                stopPump();
                Serial.println("PUMP: OFF - SAFETY! (Hypoglycemia risk)");
            }
            break;
    }
}

/*
 * Safety Task: Prevent pump from running too long
 * Real-world analog: Prevents insulin overdose
 */
void checkSafetyOverrides() {
    if (pumpActive) {
        unsigned long pumpRunTime = millis() - pumpStartTime;
        if (pumpRunTime > PUMP_SAFE_DURATION_MS) {
            stopPump();
            Serial.println("SAFETY OVERRIDE: Pump timeout - possible system fault");
        }
    }
}

/*
 * Stop pump and reset state
 */
void stopPump() {
    pumpActive = false;
    digitalWrite(PUMP_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    analogWrite(PUMP_PIN, PUMP_MIN_SPEED);
}

/*
 * Log system state for debugging and verification
 */
void logSystemStatus() {
    Serial.print("Glucose Level: ");
    Serial.print(currentGlucoseLevel);
    Serial.print(" | State: ");
    
    switch(currentState) {
        case STATE_HYPOGLYCEMIA:
            Serial.print("LOW (SAFETY)");
            break;
        case STATE_TARGET:
            Serial.print("TARGET");
            break;
        case STATE_HYPERGLYCEMIA:
            Serial.print("HIGH - Pumping");
            break;
    }
    Serial.print(" | Pump: ");
    Serial.println(pumpActive ? "ON" : "OFF");
}