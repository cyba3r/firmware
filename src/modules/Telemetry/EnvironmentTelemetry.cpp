#include "configuration.h"

#if HAS_TELEMETRY && !MESHTASTIC_EXCLUDE_ENVIRONMENTAL_SENSOR

#include "../mesh/generated/meshtastic/telemetry.pb.h"
#include "Default.h"
#include "EnvironmentTelemetry.h"
#include "MeshService.h"
#include "NodeDB.h"
#include "PowerFSM.h"
#include "RTC.h"
#include "Router.h"
#include "UnitConversions.h"
#include "buzz.h"
#include "graphics/SharedUIDisplay.h"
#include "graphics/images.h"
#include "main.h"
#include "modules/ExternalNotificationModule.h"
#include "power.h"
#include "sleep.h"
#include "target_specific.h"
#include <OLEDDisplay.h>

#if !MESHTASTIC_EXCLUDE_ENVIRONMENTAL_SENSOR_EXTERNAL

// Sensors
#include "Sensor/CGRadSensSensor.h"
#include "Sensor/RCWL9620Sensor.h"
#include "Sensor/nullSensor.h"

namespace graphics
{
extern void drawCommonHeader(OLEDDisplay *display, int16_t x, int16_t y, const char *titleStr, bool battery_only);
}
#if __has_include(<Adafruit_AHTX0.h>)
#include "Sensor/AHT10.h"
AHT10Sensor aht10Sensor;
#else
NullSensor aht10Sensor;
#endif
#if __has_include(<Adafruit_BME280.h>)
#include "Sensor/BME280Sensor.h"
BME280Sensor bme280Sensor;
#else
NullSensor bmp280Sensor;
#endif

#if __has_include(<Adafruit_BMP085.h>)
#include "Sensor/BMP085Sensor.h"
BMP085Sensor bmp085Sensor;
#else
NullSensor bmp085Sensor;
#endif

#if __has_include(<Adafruit_BMP280.h>)
#include "Sensor/BMP280Sensor.h"
BMP280Sensor bmp280Sensor;
#else
NullSensor bme280Sensor;
#endif

#if __has_include(<Adafruit_LTR390.h>)
#include "Sensor/LTR390UVSensor.h"
LTR390UVSensor ltr390uvSensor;
#else
NullSensor ltr390uvSensor;
#endif

#if __has_include(<bsec2.h>)
#include "Sensor/BME680Sensor.h"
BME680Sensor bme680Sensor;
#else
NullSensor bme680Sensor;
#endif

#if __has_include(<Adafruit_DPS310.h>)
#include "Sensor/DPS310Sensor.h"
DPS310Sensor dps310Sensor;
#else
NullSensor dps310Sensor;
#endif

#if __has_include(<Adafruit_MCP9808.h>)
#include "Sensor/MCP9808Sensor.h"
MCP9808Sensor mcp9808Sensor;
#else
NullSensor mcp9808Sensor;
#endif

#if __has_include(<Adafruit_SHT31.h>)
#include "Sensor/SHT31Sensor.h"
SHT31Sensor sht31Sensor;
#else
NullSensor sht31Sensor;
#endif

#if __has_include(<Adafruit_LPS2X.h>)
#include "Sensor/LPS22HBSensor.h"
LPS22HBSensor lps22hbSensor;
#else
NullSensor lps22hbSensor;
#endif

#if __has_include(<Adafruit_SHTC3.h>)
#include "Sensor/SHTC3Sensor.h"
SHTC3Sensor shtc3Sensor;
#else
NullSensor shtc3Sensor;
#endif

#if __has_include("RAK12035_SoilMoisture.h") && defined(RAK_4631) && RAK_4631 == 1
#include "Sensor/RAK12035Sensor.h"
RAK12035Sensor rak12035Sensor;
#else
NullSensor rak12035Sensor;
#endif

#if __has_include(<Adafruit_VEML7700.h>)
#include "Sensor/VEML7700Sensor.h"
VEML7700Sensor veml7700Sensor;
#else
NullSensor veml7700Sensor;
#endif

#if __has_include(<Adafruit_TSL2591.h>)
#include "Sensor/TSL2591Sensor.h"
TSL2591Sensor tsl2591Sensor;
#else
NullSensor tsl2591Sensor;
#endif

#if __has_include(<ClosedCube_OPT3001.h>)
#include "Sensor/OPT3001Sensor.h"
OPT3001Sensor opt3001Sensor;
#else
NullSensor opt3001Sensor;
#endif

#if __has_include(<Adafruit_SHT4x.h>)
#include "Sensor/SHT4XSensor.h"
SHT4XSensor sht4xSensor;
#else
NullSensor sht4xSensor;
#endif

#if __has_include(<SparkFun_MLX90632_Arduino_Library.h>)
#include "Sensor/MLX90632Sensor.h"
MLX90632Sensor mlx90632Sensor;
#else
NullSensor mlx90632Sensor;
#endif

#if __has_include(<DFRobot_LarkWeatherStation.h>)
#include "Sensor/DFRobotLarkSensor.h"
DFRobotLarkSensor dfRobotLarkSensor;
#else
NullSensor dfRobotLarkSensor;
#endif

#if __has_include(<DFRobot_RainfallSensor.h>)
#include "Sensor/DFRobotGravitySensor.h"
DFRobotGravitySensor dfRobotGravitySensor;
#else
NullSensor dfRobotGravitySensor;
#endif

#if __has_include(<SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h>)
#include "Sensor/NAU7802Sensor.h"
NAU7802Sensor nau7802Sensor;
#else
NullSensor nau7802Sensor;
#endif

#if __has_include(<Adafruit_BMP3XX.h>)
#include "Sensor/BMP3XXSensor.h"
BMP3XXSensor bmp3xxSensor;
#else
NullSensor bmp3xxSensor;
#endif

#if __has_include(<Adafruit_PCT2075.h>)
#include "Sensor/PCT2075Sensor.h"
PCT2075Sensor pct2075Sensor;
#else
NullSensor pct2075Sensor;
#endif

RCWL9620Sensor rcwl9620Sensor;
CGRadSensSensor cgRadSens;

#endif
#ifdef T1000X_SENSOR_EN
#include "Sensor/T1000xSensor.h"
T1000xSensor t1000xSensor;
#endif
#ifdef SENSECAP_INDICATOR
#include "Sensor/IndicatorSensor.h"
IndicatorSensor indicatorSensor;
#endif

#define FAILED_STATE_SENSOR_READ_MULTIPLIER 10
#define DISPLAY_RECEIVEID_MEASUREMENTS_ON_SCREEN true

#include "graphics/ScreenFonts.h"
#include <Throttle.h>

int32_t EnvironmentTelemetryModule::runOnce()
{
    if (sleepOnNextExecution == true) {
        sleepOnNextExecution = false;
        uint32_t nightyNightMs = Default::getConfiguredOrDefaultMs(moduleConfig.telemetry.environment_update_interval,
                                                                   default_telemetry_broadcast_interval_secs);
        LOG_DEBUG("Sleep for %ims, then awake to send metrics again", nightyNightMs);
        doDeepSleep(nightyNightMs, true, false);
    }

    uint32_t result = UINT32_MAX;
    /*
        Uncomment the preferences below if you want to use the module
        without having to configure it from the PythonAPI or WebUI.
    */

    // moduleConfig.telemetry.environment_measurement_enabled = 1;
    // moduleConfig.telemetry.environment_screen_enabled = 1;
    // moduleConfig.telemetry.environment_update_interval = 15;

    if (!(moduleConfig.telemetry.environment_measurement_enabled || moduleConfig.telemetry.environment_screen_enabled ||
          ENVIRONMENTAL_TELEMETRY_MODULE_ENABLE)) {
        // If this module is not enabled, and the user doesn't want the display screen don't waste any OSThread time on it
        return disable();
    }

    if (firstTime) {
        // This is the first time the OSThread library has called this function, so do some setup
        firstTime = 0;

        if (moduleConfig.telemetry.environment_measurement_enabled || ENVIRONMENTAL_TELEMETRY_MODULE_ENABLE) {
            LOG_INFO("Environment Telemetry: init");
#ifdef SENSECAP_INDICATOR
            result = indicatorSensor.runOnce();
#endif
#ifdef T1000X_SENSOR_EN
            result = t1000xSensor.runOnce();
#elif !MESHTASTIC_EXCLUDE_ENVIRONMENTAL_SENSOR_EXTERNAL
            if (dfRobotLarkSensor.hasSensor())
                result = dfRobotLarkSensor.runOnce();
            if (dfRobotGravitySensor.hasSensor())
                result = dfRobotGravitySensor.runOnce();
            if (bmp085Sensor.hasSensor())
                result = bmp085Sensor.runOnce();
#if __has_include(<Adafruit_BME280.h>)
            if (bmp280Sensor.hasSensor())
                result = bmp280Sensor.runOnce();
#endif
            if (bme280Sensor.hasSensor())
                result = bme280Sensor.runOnce();
            if (ltr390uvSensor.hasSensor())
                result = ltr390uvSensor.runOnce();
            if (bmp3xxSensor.hasSensor())
                result = bmp3xxSensor.runOnce();
            if (bme680Sensor.hasSensor())
                result = bme680Sensor.runOnce();
            if (dps310Sensor.hasSensor())
                result = dps310Sensor.runOnce();
            if (mcp9808Sensor.hasSensor())
                result = mcp9808Sensor.runOnce();
            if (shtc3Sensor.hasSensor())
                result = shtc3Sensor.runOnce();
            if (lps22hbSensor.hasSensor())
                result = lps22hbSensor.runOnce();
            if (sht31Sensor.hasSensor())
                result = sht31Sensor.runOnce();
            if (sht4xSensor.hasSensor())
                result = sht4xSensor.runOnce();
            if (ina219Sensor.hasSensor())
                result = ina219Sensor.runOnce();
            if (ina260Sensor.hasSensor())
                result = ina260Sensor.runOnce();
            if (ina3221Sensor.hasSensor())
                result = ina3221Sensor.runOnce();
            if (veml7700Sensor.hasSensor())
                result = veml7700Sensor.runOnce();
            if (tsl2591Sensor.hasSensor())
                result = tsl2591Sensor.runOnce();
            if (opt3001Sensor.hasSensor())
                result = opt3001Sensor.runOnce();
            if (rcwl9620Sensor.hasSensor())
                result = rcwl9620Sensor.runOnce();
            if (aht10Sensor.hasSensor())
                result = aht10Sensor.runOnce();
            if (mlx90632Sensor.hasSensor())
                result = mlx90632Sensor.runOnce();
            if (nau7802Sensor.hasSensor())
                result = nau7802Sensor.runOnce();
            if (max17048Sensor.hasSensor())
                result = max17048Sensor.runOnce();
            if (cgRadSens.hasSensor())
                result = cgRadSens.runOnce();
            if (pct2075Sensor.hasSensor())
                result = pct2075Sensor.runOnce();
                // this only works on the wismesh hub with the solar option. This is not an I2C sensor, so we don't need the
                // sensormap here.
#ifdef HAS_RAKPROT

            result = rak9154Sensor.runOnce();
#endif
#if __has_include("RAK12035_SoilMoisture.h") && defined(RAK_4631) && RAK_4631 == 1
            if (rak12035Sensor.hasSensor()) {
                result = rak12035Sensor.runOnce();
            }
#endif
#endif
        }
        // it's possible to have this module enabled, only for displaying values on the screen.
        // therefore, we should only enable the sensor loop if measurement is also enabled
        return result == UINT32_MAX ? disable() : setStartDelay();
    } else {
        // if we somehow got to a second run of this module with measurement disabled, then just wait forever
        if (!moduleConfig.telemetry.environment_measurement_enabled && !ENVIRONMENTAL_TELEMETRY_MODULE_ENABLE) {
            return disable();
        } else {
#if !MESHTASTIC_EXCLUDE_ENVIRONMENTAL_SENSOR_EXTERNAL
            if (bme680Sensor.hasSensor())
                result = bme680Sensor.runTrigger();
#endif
        }

        if (((lastSentToMesh == 0) ||
             !Throttle::isWithinTimespanMs(lastSentToMesh, Default::getConfiguredOrDefaultMsScaled(
                                                               moduleConfig.telemetry.environment_update_interval,
                                                               default_telemetry_broadcast_interval_secs, numOnlineNodes))) &&
            airTime->isTxAllowedChannelUtil(config.device.role != meshtastic_Config_DeviceConfig_Role_SENSOR) &&
            airTime->isTxAllowedAirUtil()) {
            sendTelemetry();
            lastSentToMesh = millis();
        } else if (((lastSentToPhone == 0) || !Throttle::isWithinTimespanMs(lastSentToPhone, sendToPhoneIntervalMs)) &&
                   (service->isToPhoneQueueEmpty())) {
            // Just send to phone when it's not our time to send to mesh yet
            // Only send while queue is empty (phone assumed connected)
            sendTelemetry(NODENUM_BROADCAST, true);
            lastSentToPhone = millis();
        }
    }
    return min(sendToPhoneIntervalMs, result);
}

bool EnvironmentTelemetryModule::wantUIFrame()
{
    return moduleConfig.telemetry.environment_screen_enabled;
}

void EnvironmentTelemetryModule::drawFrame(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    // === Setup display ===
    display->clear();
    display->setFont(FONT_SMALL);
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    int line = 1;

    // === Set Title
    const char *titleStr = (graphics::isHighResolution) ? "Environment" : "Env.";

    // === Header ===
    graphics::drawCommonHeader(display, x, y, titleStr);

    // === Row spacing setup ===
    const int rowHeight = FONT_HEIGHT_SMALL - 4;
    int currentY = graphics::getTextPositions(display)[line++];

    // === Show "No Telemetry" if no data available ===
    if (!lastMeasurementPacket) {
        display->drawString(x, currentY, "No Telemetry");
        return;
    }

    // Decode the telemetry message from the latest received packet
    const meshtastic_Data &p = lastMeasurementPacket->decoded;
    meshtastic_Telemetry telemetry;
    if (!pb_decode_from_bytes(p.payload.bytes, p.payload.size, &meshtastic_Telemetry_msg, &telemetry)) {
        display->drawString(x, currentY, "No Telemetry");
        return;
    }

    const auto &m = telemetry.variant.environment_metrics;

    // Check if any telemetry field has valid data
    bool hasAny = m.has_temperature || m.has_relative_humidity || m.barometric_pressure != 0 || m.iaq != 0 || m.voltage != 0 ||
                  m.current != 0 || m.lux != 0 || m.white_lux != 0 || m.weight != 0 || m.distance != 0 || m.radiation != 0;

    if (!hasAny) {
        display->drawString(x, currentY, "No Telemetry");
        return;
    }

    // === First line: Show sender name + time since received (left), and first metric (right) ===
    const char *sender = getSenderShortName(*lastMeasurementPacket);
    uint32_t agoSecs = service->GetTimeSinceMeshPacket(lastMeasurementPacket);
    String agoStr = (agoSecs > 864000) ? "?"
                    : (agoSecs > 3600) ? String(agoSecs / 3600) + "h"
                    : (agoSecs > 60)   ? String(agoSecs / 60) + "m"
                                       : String(agoSecs) + "s";

    String leftStr = String(sender) + " (" + agoStr + ")";
    display->drawString(x, currentY, leftStr); // Left side: who and when

    // === Collect sensor readings as label strings (no icons) ===
    std::vector<String> entries;

    if (m.has_temperature) {
        String tempStr = moduleConfig.telemetry.environment_display_fahrenheit
                             ? "Tmp: " + String(UnitConversions::CelsiusToFahrenheit(m.temperature), 1) + "°F"
                             : "Tmp: " + String(m.temperature, 1) + "°C";
        entries.push_back(tempStr);
    }
    if (m.has_relative_humidity)
        entries.push_back("Hum: " + String(m.relative_humidity, 0) + "%");
    if (m.barometric_pressure != 0)
        entries.push_back("Prss: " + String(m.barometric_pressure, 0) + " hPa");
    if (m.iaq != 0) {
        String aqi = "IAQ: " + String(m.iaq);
        const char *bannerMsg = nullptr; // Default: no banner

        if (m.iaq <= 25)
            aqi += " (Excellent)";
        else if (m.iaq <= 50)
            aqi += " (Good)";
        else if (m.iaq <= 100)
            aqi += " (Moderate)";
        else if (m.iaq <= 150)
            aqi += " (Poor)";
        else if (m.iaq <= 200) {
            aqi += " (Unhealthy)";
            bannerMsg = "Unhealthy IAQ";
        } else if (m.iaq <= 300) {
            aqi += " (Very Unhealthy)";
            bannerMsg = "Very Unhealthy IAQ";
        } else {
            aqi += " (Hazardous)";
            bannerMsg = "Hazardous IAQ";
        }

        entries.push_back(aqi);

        // === IAQ alert logic ===
        static uint32_t lastAlertTime = 0;
        uint32_t now = millis();

        bool isOwnTelemetry = lastMeasurementPacket->from == nodeDB->getNodeNum();
        bool isCooldownOver = (now - lastAlertTime > 60000);

        if (isOwnTelemetry && bannerMsg && isCooldownOver) {
            LOG_INFO("drawFrame: IAQ %d (own) — showing banner: %s", m.iaq, bannerMsg);
            screen->showSimpleBanner(bannerMsg, 3000);

            // Only buzz if IAQ is over 200
            if (m.iaq > 200 && moduleConfig.external_notification.enabled && !externalNotificationModule->getMute()) {
                playLongBeep();
            }

            lastAlertTime = now;
        }
    }
    if (m.voltage != 0 || m.current != 0)
        entries.push_back(String(m.voltage, 1) + "V / " + String(m.current, 0) + "mA");
    if (m.lux != 0)
        entries.push_back("Light: " + String(m.lux, 0) + "lx");
    if (m.white_lux != 0)
        entries.push_back("White: " + String(m.white_lux, 0) + "lx");
    if (m.weight != 0)
        entries.push_back("Weight: " + String(m.weight, 0) + "kg");
    if (m.distance != 0)
        entries.push_back("Level: " + String(m.distance, 0) + "mm");
    if (m.radiation != 0)
        entries.push_back("Rad: " + String(m.radiation, 2) + " µR/h");

    // === Show first available metric on top-right of first line ===
    if (!entries.empty()) {
        String valueStr = entries.front();
        int rightX = SCREEN_WIDTH - display->getStringWidth(valueStr);
        display->drawString(rightX, currentY, valueStr);
        entries.erase(entries.begin()); // Remove from queue
    }

    // === Advance to next line for remaining telemetry entries ===
    currentY += rowHeight;

    // === Draw remaining entries in 2-column format (left and right) ===
    for (size_t i = 0; i < entries.size(); i += 2) {
        // Left column
        display->drawString(x, currentY, entries[i]);

        // Right column if it exists
        if (i + 1 < entries.size()) {
            int rightX = SCREEN_WIDTH / 2;
            display->drawString(rightX, currentY, entries[i + 1]);
        }

        currentY += rowHeight;
    }
}

bool EnvironmentTelemetryModule::handleReceivedProtobuf(const meshtastic_MeshPacket &mp, meshtastic_Telemetry *t)
{
    if (t->which_variant == meshtastic_Telemetry_environment_metrics_tag) {
#if defined(DEBUG_PORT) && !defined(DEBUG_MUTE)
        const char *sender = getSenderShortName(mp);

        LOG_INFO("(Received from %s): barometric_pressure=%f, current=%f, gas_resistance=%f, relative_humidity=%f, "
                 "temperature=%f",
                 sender, t->variant.environment_metrics.barometric_pressure, t->variant.environment_metrics.current,
                 t->variant.environment_metrics.gas_resistance, t->variant.environment_metrics.relative_humidity,
                 t->variant.environment_metrics.temperature);
        LOG_INFO("(Received from %s): voltage=%f, IAQ=%d, distance=%f, lux=%f, white_lux=%f", sender,
                 t->variant.environment_metrics.voltage, t->variant.environment_metrics.iaq,
                 t->variant.environment_metrics.distance, t->variant.environment_metrics.lux,
                 t->variant.environment_metrics.white_lux);

        LOG_INFO("(Received from %s): wind speed=%fm/s, direction=%d degrees, weight=%fkg", sender,
                 t->variant.environment_metrics.wind_speed, t->variant.environment_metrics.wind_direction,
                 t->variant.environment_metrics.weight);

        LOG_INFO("(Received from %s): radiation=%fµR/h", sender, t->variant.environment_metrics.radiation);

#endif
        // release previous packet before occupying a new spot
        if (lastMeasurementPacket != nullptr)
            packetPool.release(lastMeasurementPacket);

        lastMeasurementPacket = packetPool.allocCopy(mp);
    }

    return false; // Let others look at this message also if they want
}

bool EnvironmentTelemetryModule::getEnvironmentTelemetry(meshtastic_Telemetry *m)
{
    bool valid = true;
    bool hasSensor = false;
    m->time = getTime();
    m->which_variant = meshtastic_Telemetry_environment_metrics_tag;
    m->variant.environment_metrics = meshtastic_EnvironmentMetrics_init_zero;

#ifdef SENSECAP_INDICATOR
    valid = valid && indicatorSensor.getMetrics(m);
    hasSensor = true;
#endif
#ifdef T1000X_SENSOR_EN // add by WayenWeng
    valid = valid && t1000xSensor.getMetrics(m);
    hasSensor = true;
#else
    if (dfRobotLarkSensor.hasSensor()) {
        valid = valid && dfRobotLarkSensor.getMetrics(m);
        hasSensor = true;
    }
    if (dfRobotGravitySensor.hasSensor()) {
        valid = valid && dfRobotGravitySensor.getMetrics(m);
        hasSensor = true;
    }
    if (sht31Sensor.hasSensor()) {
        valid = valid && sht31Sensor.getMetrics(m);
        hasSensor = true;
    }
    if (sht4xSensor.hasSensor()) {
        valid = valid && sht4xSensor.getMetrics(m);
        hasSensor = true;
    }
    if (lps22hbSensor.hasSensor()) {
        valid = valid && lps22hbSensor.getMetrics(m);
        hasSensor = true;
    }
    if (shtc3Sensor.hasSensor()) {
        valid = valid && shtc3Sensor.getMetrics(m);
        hasSensor = true;
    }
    if (bmp085Sensor.hasSensor()) {
        valid = valid && bmp085Sensor.getMetrics(m);
        hasSensor = true;
    }
#if __has_include(<Adafruit_BME280.h>)
    if (bmp280Sensor.hasSensor()) {
        valid = valid && bmp280Sensor.getMetrics(m);
        hasSensor = true;
    }
#endif
    if (bme280Sensor.hasSensor()) {
        valid = valid && bme280Sensor.getMetrics(m);
        hasSensor = true;
    }
    if (ltr390uvSensor.hasSensor()) {
        valid = valid && ltr390uvSensor.getMetrics(m);
        hasSensor = true;
    }
    if (bmp3xxSensor.hasSensor()) {
        valid = valid && bmp3xxSensor.getMetrics(m);
        hasSensor = true;
    }
    if (bme680Sensor.hasSensor()) {
        valid = valid && bme680Sensor.getMetrics(m);
        hasSensor = true;
    }
    if (dps310Sensor.hasSensor()) {
        valid = valid && dps310Sensor.getMetrics(m);
        hasSensor = true;
    }
    if (mcp9808Sensor.hasSensor()) {
        valid = valid && mcp9808Sensor.getMetrics(m);
        hasSensor = true;
    }
    if (ina219Sensor.hasSensor()) {
        valid = valid && ina219Sensor.getMetrics(m);
        hasSensor = true;
    }
    if (ina260Sensor.hasSensor()) {
        valid = valid && ina260Sensor.getMetrics(m);
        hasSensor = true;
    }
    if (ina3221Sensor.hasSensor()) {
        valid = valid && ina3221Sensor.getMetrics(m);
        hasSensor = true;
    }
    if (veml7700Sensor.hasSensor()) {
        valid = valid && veml7700Sensor.getMetrics(m);
        hasSensor = true;
    }
    if (tsl2591Sensor.hasSensor()) {
        valid = valid && tsl2591Sensor.getMetrics(m);
        hasSensor = true;
    }
    if (opt3001Sensor.hasSensor()) {
        valid = valid && opt3001Sensor.getMetrics(m);
        hasSensor = true;
    }
    if (mlx90632Sensor.hasSensor()) {
        valid = valid && mlx90632Sensor.getMetrics(m);
        hasSensor = true;
    }
    if (rcwl9620Sensor.hasSensor()) {
        valid = valid && rcwl9620Sensor.getMetrics(m);
        hasSensor = true;
    }
    if (nau7802Sensor.hasSensor()) {
        valid = valid && nau7802Sensor.getMetrics(m);
        hasSensor = true;
    }
    if (aht10Sensor.hasSensor()) {
        if (!bmp280Sensor.hasSensor() && !bmp3xxSensor.hasSensor()) {
            valid = valid && aht10Sensor.getMetrics(m);
            hasSensor = true;
        } else if (bmp280Sensor.hasSensor()) {
            // prefer bmp280 temp if both sensors are present, fetch only humidity
            meshtastic_Telemetry m_ahtx = meshtastic_Telemetry_init_zero;
            LOG_INFO("AHTX0+BMP280 module detected: using temp from BMP280 and humy from AHTX0");
            aht10Sensor.getMetrics(&m_ahtx);
            m->variant.environment_metrics.relative_humidity = m_ahtx.variant.environment_metrics.relative_humidity;
            m->variant.environment_metrics.has_relative_humidity = m_ahtx.variant.environment_metrics.has_relative_humidity;
        } else {
            // prefer bmp3xx temp if both sensors are present, fetch only humidity
            meshtastic_Telemetry m_ahtx = meshtastic_Telemetry_init_zero;
            LOG_INFO("AHTX0+BMP3XX module detected: using temp from BMP3XX and humy from AHTX0");
            aht10Sensor.getMetrics(&m_ahtx);
            m->variant.environment_metrics.relative_humidity = m_ahtx.variant.environment_metrics.relative_humidity;
            m->variant.environment_metrics.has_relative_humidity = m_ahtx.variant.environment_metrics.has_relative_humidity;
        }
    }
    if (max17048Sensor.hasSensor()) {
        valid = valid && max17048Sensor.getMetrics(m);
        hasSensor = true;
    }
    if (cgRadSens.hasSensor()) {
        valid = valid && cgRadSens.getMetrics(m);
        hasSensor = true;
    }
    if (pct2075Sensor.hasSensor()) {
        valid = valid && pct2075Sensor.getMetrics(m);
        hasSensor = true;
    }
#ifdef HAS_RAKPROT
    valid = valid && rak9154Sensor.getMetrics(m);
    hasSensor = true;
#endif
#if __has_include("RAK12035_SoilMoisture.h") && defined(RAK_4631) &&                                                             \
                  RAK_4631 ==                                                                                                    \
                      1 // Not really needed, but may as well just skip at a lower level it if no library or not a RAK_4631
    if (rak12035Sensor.hasSensor()) {
        valid = valid && rak12035Sensor.getMetrics(m);
        hasSensor = true;
    }
#endif
#endif
    return valid && hasSensor;
}

meshtastic_MeshPacket *EnvironmentTelemetryModule::allocReply()
{
    if (currentRequest) {
        auto req = *currentRequest;
        const auto &p = req.decoded;
        meshtastic_Telemetry scratch;
        meshtastic_Telemetry *decoded = NULL;
        memset(&scratch, 0, sizeof(scratch));
        if (pb_decode_from_bytes(p.payload.bytes, p.payload.size, &meshtastic_Telemetry_msg, &scratch)) {
            decoded = &scratch;
        } else {
            LOG_ERROR("Error decoding EnvironmentTelemetry module!");
            return NULL;
        }
        // Check for a request for environment metrics
        if (decoded->which_variant == meshtastic_Telemetry_environment_metrics_tag) {
            meshtastic_Telemetry m = meshtastic_Telemetry_init_zero;
            if (getEnvironmentTelemetry(&m)) {
                LOG_INFO("Environment telemetry reply to request");
                return allocDataProtobuf(m);
            } else {
                return NULL;
            }
        }
    }
    return NULL;
}

bool EnvironmentTelemetryModule::sendTelemetry(NodeNum dest, bool phoneOnly)
{
    meshtastic_Telemetry m = meshtastic_Telemetry_init_zero;
    m.which_variant = meshtastic_Telemetry_environment_metrics_tag;
    m.time = getTime();
#ifdef T1000X_SENSOR_EN
    if (t1000xSensor.getMetrics(&m)) {
#else
    if (getEnvironmentTelemetry(&m)) {
#endif
        LOG_INFO("Send: barometric_pressure=%f, current=%f, gas_resistance=%f, relative_humidity=%f, temperature=%f",
                 m.variant.environment_metrics.barometric_pressure, m.variant.environment_metrics.current,
                 m.variant.environment_metrics.gas_resistance, m.variant.environment_metrics.relative_humidity,
                 m.variant.environment_metrics.temperature);
        LOG_INFO("Send: voltage=%f, IAQ=%d, distance=%f, lux=%f", m.variant.environment_metrics.voltage,
                 m.variant.environment_metrics.iaq, m.variant.environment_metrics.distance, m.variant.environment_metrics.lux);

        LOG_INFO("Send: wind speed=%fm/s, direction=%d degrees, weight=%fkg", m.variant.environment_metrics.wind_speed,
                 m.variant.environment_metrics.wind_direction, m.variant.environment_metrics.weight);

        LOG_INFO("Send: radiation=%fµR/h", m.variant.environment_metrics.radiation);

        LOG_INFO("Send: soil_temperature=%f, soil_moisture=%u", m.variant.environment_metrics.soil_temperature,
                 m.variant.environment_metrics.soil_moisture);

        sensor_read_error_count = 0;

        meshtastic_MeshPacket *p = allocDataProtobuf(m);
        p->to = dest;
        p->decoded.want_response = false;
        if (config.device.role == meshtastic_Config_DeviceConfig_Role_SENSOR)
            p->priority = meshtastic_MeshPacket_Priority_RELIABLE;
        else
            p->priority = meshtastic_MeshPacket_Priority_BACKGROUND;
        // release previous packet before occupying a new spot
        if (lastMeasurementPacket != nullptr)
            packetPool.release(lastMeasurementPacket);

        lastMeasurementPacket = packetPool.allocCopy(*p);
        if (phoneOnly) {
            LOG_INFO("Send packet to phone");
            service->sendToPhone(p);
        } else {
            LOG_INFO("Send packet to mesh");
            service->sendToMesh(p, RX_SRC_LOCAL, true);

            if (config.device.role == meshtastic_Config_DeviceConfig_Role_SENSOR && config.power.is_power_saving) {
                meshtastic_ClientNotification *notification = clientNotificationPool.allocZeroed();
                notification->level = meshtastic_LogRecord_Level_INFO;
                notification->time = getValidTime(RTCQualityFromNet);
                sprintf(notification->message, "Sending telemetry and sleeping for %us interval in a moment",
                        Default::getConfiguredOrDefaultMs(moduleConfig.telemetry.environment_update_interval,
                                                          default_telemetry_broadcast_interval_secs) /
                            1000U);
                service->sendClientNotification(notification);
                sleepOnNextExecution = true;
                LOG_DEBUG("Start next execution in 5s, then sleep");
                setIntervalFromNow(FIVE_SECONDS_MS);
            }
        }
        return true;
    }
    return false;
}

AdminMessageHandleResult EnvironmentTelemetryModule::handleAdminMessageForModule(const meshtastic_MeshPacket &mp,
                                                                                 meshtastic_AdminMessage *request,
                                                                                 meshtastic_AdminMessage *response)
{
    AdminMessageHandleResult result = AdminMessageHandleResult::NOT_HANDLED;
#if !MESHTASTIC_EXCLUDE_ENVIRONMENTAL_SENSOR_EXTERNAL
    if (dfRobotLarkSensor.hasSensor()) {
        result = dfRobotLarkSensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (dfRobotGravitySensor.hasSensor()) {
        result = dfRobotGravitySensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (sht31Sensor.hasSensor()) {
        result = sht31Sensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (lps22hbSensor.hasSensor()) {
        result = lps22hbSensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (shtc3Sensor.hasSensor()) {
        result = shtc3Sensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (bmp085Sensor.hasSensor()) {
        result = bmp085Sensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (bmp280Sensor.hasSensor()) {
        result = bmp280Sensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (bme280Sensor.hasSensor()) {
        result = bme280Sensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (ltr390uvSensor.hasSensor()) {
        result = ltr390uvSensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (bmp3xxSensor.hasSensor()) {
        result = bmp3xxSensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (bme680Sensor.hasSensor()) {
        result = bme680Sensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (dps310Sensor.hasSensor()) {
        result = dps310Sensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (mcp9808Sensor.hasSensor()) {
        result = mcp9808Sensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (ina219Sensor.hasSensor()) {
        result = ina219Sensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (ina260Sensor.hasSensor()) {
        result = ina260Sensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (ina3221Sensor.hasSensor()) {
        result = ina3221Sensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (veml7700Sensor.hasSensor()) {
        result = veml7700Sensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (tsl2591Sensor.hasSensor()) {
        result = tsl2591Sensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (opt3001Sensor.hasSensor()) {
        result = opt3001Sensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (mlx90632Sensor.hasSensor()) {
        result = mlx90632Sensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (rcwl9620Sensor.hasSensor()) {
        result = rcwl9620Sensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (nau7802Sensor.hasSensor()) {
        result = nau7802Sensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (aht10Sensor.hasSensor()) {
        result = aht10Sensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (max17048Sensor.hasSensor()) {
        result = max17048Sensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
    if (cgRadSens.hasSensor()) {
        result = cgRadSens.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
#if __has_include("RAK12035_SoilMoisture.h") && defined(RAK_4631) &&                                                             \
                  RAK_4631 ==                                                                                                    \
                      1 // Not really needed, but may as well just skip it at a lower level if no library or not a RAK_4631
    if (rak12035Sensor.hasSensor()) {
        result = rak12035Sensor.handleAdminMessage(mp, request, response);
        if (result != AdminMessageHandleResult::NOT_HANDLED)
            return result;
    }
#endif
#endif
    return result;
}

#endif
