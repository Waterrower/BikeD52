#include <progmem.h>
#include "AddStrings.h"
//#define SPRINTBOK
//#define BIKE
#define CICLOTTE

const char string1_1[] PROGMEM = "";
const char string1_2[] PROGMEM = "Watts";
#ifdef CICLOTTE
const char string1_3[] PROGMEM = "Gear";
#else
const char string1_3[] PROGMEM = "Resistance";
#endif

const char string1_4[] PROGMEM = "rpm";
const char string1_5[] PROGMEM = "Time";
const char string1_6[] PROGMEM = "Distance";
const char string1_7[] PROGMEM = "Just Cycle";
const char string1_8[] PROGMEM = "Settings";
const char string1_9[] PROGMEM = "km/h";
const char string1_10[] PROGMEM = "Watts";
const char string1_11[] PROGMEM = "Distance/m";
const char string1_12[] PROGMEM = "Minutes";
const char string1_13[] PROGMEM = "Go";
const char string1_14[] PROGMEM = "Go Back";
const char string1_15[] PROGMEM = "Hours";
const char string1_16[] PROGMEM = "BPM";
const char string1_17[] PROGMEM = "Kcal";
const char string1_18[] PROGMEM = "Pace/km";
const char string1_19[] PROGMEM = "English(km)";
const char string1_20[] PROGMEM = "Light Mode";
const char string1_21[] PROGMEM = "Dark Mode";
const char string1_22[] PROGMEM = "Connect HR";
const char string1_23[] PROGMEM = "Next";
const char string1_24[] PROGMEM = "Home";
const char string1_25[] PROGMEM = "BT off";
const char string1_26[] PROGMEM = "BT on";
const char string1_27[] PROGMEM = "Info";
const char string1_28[] PROGMEM = "Calibration";
const char string1_29[] PROGMEM = "Just Run";
const char string1_30[] PROGMEM = "For more";
const char string1_31[] PROGMEM = "Inform.";
const char string1_32[] PROGMEM = "Scan";
const char string1_33[] PROGMEM = "QR-Code";
const char string1_34[] PROGMEM = "below";
const char string1_35[] PROGMEM = "HR";
const char string1_36[] PROGMEM = "disabled";
const char string1_37[] PROGMEM = "No";
const char string1_38[] PROGMEM = "Sensors";
const char string1_39[] PROGMEM = "found";
const char string1_40[] PROGMEM = "connected";
const char string1_41[] PROGMEM = "disconnect";
const char string1_42[] PROGMEM = "connected";
const char string1_43[] PROGMEM = "to";
const char string1_44[] PROGMEM = "App";
const char string1_45[] PROGMEM = "Exit";
const char string1_46[] PROGMEM = "No";
const char string1_47[] PROGMEM = "Yes";
const char string1_48[] PROGMEM = "Quit?";
const char string1_49[] PROGMEM = "paused";
const char string1_50[] PROGMEM = "{ km/h";
const char string1_51[] PROGMEM = "{ Pace";
const char string1_52[] PROGMEM = "{ BPM";
const char string1_53[] PROGMEM = "max BPM";
const char string1_54[] PROGMEM = "top speed";
const char string1_55[] PROGMEM = "Summary";
const char string1_56[] PROGMEM = "{ Watts";
const char string1_57[] PROGMEM = "Turn the";
const char string1_58[] PROGMEM = "resistance";
const char string1_59[] PROGMEM = "knob to the";
const char string1_60[] PROGMEM = "right";
const char string1_61[] PROGMEM = "all the way";
const char string1_62[] PROGMEM = "left";
const char string1_63[] PROGMEM = "all the way";
const char string1_64[] PROGMEM = "Calibration";
const char string1_65[] PROGMEM = "completed.";
const char string1_66[] PROGMEM = "Step";

const char string1_67[] PROGMEM = "Proj.";
const char string1_68[] PROGMEM = "Contrast";
const char string1_69[] PROGMEM = "High";
const char string1_70[] PROGMEM = "Mid";
const char string1_71[] PROGMEM = "Low";
const char string1_72[] PROGMEM = "Customize";
const char string1_73[] PROGMEM = "Setup";
const char string1_74[] PROGMEM = "Battery";
const char string1_75[] PROGMEM = "Grid";


const char string2_1[] PROGMEM = ""; // Empty string at index 1
const char string2_2[] PROGMEM = "Watt";
#ifdef CICLOTTE
const char string2_3[] PROGMEM = "Gang";
#else
const char string2_3[] PROGMEM = "Widerstand";
#endif

const char string2_4[] PROGMEM = "U/min";
const char string2_5[] PROGMEM = "Zeit";
const char string2_6[] PROGMEM = "Distanz";
const char string2_7[] PROGMEM = "Freie Fahrt";
const char string2_8[] PROGMEM = "Einstellung";
const char string2_9[] PROGMEM = "Km/h";
const char string2_10[] PROGMEM = "Watt";
const char string2_11[] PROGMEM = "Distanz/m";
const char string2_12[] PROGMEM = "Minuten";
const char string2_13[] PROGMEM = "Start";
const char string2_14[] PROGMEM = "Zur}ck";
const char string2_15[] PROGMEM = "Stunden";
const char string2_16[] PROGMEM = "HF";
const char string2_17[] PROGMEM = "Kcal";
const char string2_18[] PROGMEM = "Pace/km";
const char string2_19[] PROGMEM = "Deutsch";
const char string2_20[] PROGMEM = "Hell";
const char string2_21[] PROGMEM = "Dunkel";
const char string2_22[] PROGMEM = "HF Sensor";
const char string2_23[] PROGMEM = "N|chste";
const char string2_24[] PROGMEM = "Hauptmen}";
const char string2_25[] PROGMEM = "BT aus";
const char string2_26[] PROGMEM = "BT an";
const char string2_27[] PROGMEM = "Information";
const char string2_28[] PROGMEM = "Kalibrieren";
const char string2_29[] PROGMEM = "Laufen";
const char string2_30[] PROGMEM = "Für mehr";
const char string2_31[] PROGMEM = "Infos";
const char string2_32[] PROGMEM = "Scannen";
const char string2_33[] PROGMEM = "Sie den";
const char string2_34[] PROGMEM = "QR-Code";
const char string2_35[] PROGMEM = "HF";
const char string2_36[] PROGMEM = "Deaktiviert";
const char string2_37[] PROGMEM = "Kein";
const char string2_38[] PROGMEM = "Sensor";
const char string2_39[] PROGMEM = "Gefunden";
const char string2_40[] PROGMEM = "Verbunden";
const char string2_41[] PROGMEM = "Trennen";
const char string2_42[] PROGMEM = "Verbunden";
const char string2_43[] PROGMEM = "Mit";
const char string2_44[] PROGMEM = "App";
const char string2_45[] PROGMEM = "Ende";
const char string2_46[] PROGMEM = "Nein";
const char string2_47[] PROGMEM = "Ja";
const char string2_48[] PROGMEM = "Beenden?";
const char string2_49[] PROGMEM = "Pause";
const char string2_50[] PROGMEM = "{ km/h";
const char string2_51[] PROGMEM = "{ Pace";
const char string2_52[] PROGMEM = "{ HF";
const char string2_53[] PROGMEM = "max HF";
const char string2_54[] PROGMEM = "max km/h";
const char string2_55[] PROGMEM = "Ergebnis";
const char string2_56[] PROGMEM = "{ Watt";
const char string2_57[] PROGMEM = "Stellen";
const char string2_58[] PROGMEM = "Sie den";
const char string2_59[] PROGMEM = "Widerstand";
const char string2_60[] PROGMEM = "auf Max.";
const char string2_61[] PROGMEM = "bis Anschlag";
const char string2_62[] PROGMEM = "auf Min.";
const char string2_63[] PROGMEM = "bis Anschlag";
const char string2_64[] PROGMEM = "Kalibration";
const char string2_65[] PROGMEM = "fertig.";
const char string2_66[] PROGMEM = "Schritt";

const char string2_67[] PROGMEM = "Proj.";
const char string2_68[] PROGMEM = "Kontrast";
const char string2_69[] PROGMEM = "Hoch";
const char string2_70[] PROGMEM = "Mittel";
const char string2_71[] PROGMEM = "Niedrig";
const char string2_72[] PROGMEM = "Anpassen";
const char string2_73[] PROGMEM = "Setup";
const char string2_74[] PROGMEM = "Akku";
const char string2_75[] PROGMEM = "Netz";
	

const char string3_1[] PROGMEM = "";
const char string3_2[] PROGMEM = "Watts";
#ifdef CICLOTTE
const char string3_3[] PROGMEM = "Vitesse";
#else
const char string3_3[] PROGMEM = "Resistance";
#endif

const char string3_4[] PROGMEM = "Tr/Min";
const char string3_5[] PROGMEM = "Temps";
const char string3_6[] PROGMEM = "Distance";
const char string3_7[] PROGMEM = "V$lo libre";
const char string3_8[] PROGMEM = "Param#tres";
const char string3_9[] PROGMEM = "Km/h";
const char string3_10[] PROGMEM = "Watts";
const char string3_11[] PROGMEM = "Distance/m";
const char string3_12[] PROGMEM = "minutes";
const char string3_13[] PROGMEM = "commencer";
const char string3_14[] PROGMEM = "reour";
const char string3_15[] PROGMEM = "heures";
const char string3_16[] PROGMEM = "BPM";
const char string3_17[] PROGMEM = "Kcal";
const char string3_18[] PROGMEM = "Allure au km";
const char string3_19[] PROGMEM = "Francais";
const char string3_20[] PROGMEM = "Mode clair";
const char string3_21[] PROGMEM = "Mode sombre";
const char string3_22[] PROGMEM = "Capteur HF";
const char string3_23[] PROGMEM = "Suivant";
const char string3_24[] PROGMEM = "principal";
const char string3_25[] PROGMEM = "BT d$sactiv$";
const char string3_26[] PROGMEM = "BT activ$";
const char string3_27[] PROGMEM = "Informations";
const char string3_28[] PROGMEM = "Calibration";
const char string3_29[] PROGMEM = "le jog";
const char string3_30[] PROGMEM = "Pour plus";
const char string3_31[] PROGMEM = "d'informations";
const char string3_32[] PROGMEM = "scannez";
const char string3_33[] PROGMEM = "le";
const char string3_34[] PROGMEM = "code QR";
const char string3_35[] PROGMEM = "HR";
const char string3_36[] PROGMEM = "d$sactiv$";
const char string3_37[] PROGMEM = "Aucun";
const char string3_38[] PROGMEM = "Capteur";
const char string3_39[] PROGMEM = "trouv$";
const char string3_40[] PROGMEM = "connect$";
const char string3_41[] PROGMEM = "s$parer";
const char string3_42[] PROGMEM = "connect$";
const char string3_43[] PROGMEM = "a";
const char string3_44[] PROGMEM = "l'App";
const char string3_45[] PROGMEM = "Fin";
const char string3_46[] PROGMEM = "No";
const char string3_47[] PROGMEM = "Oui";
const char string3_48[] PROGMEM = " terminer?";
const char string3_49[] PROGMEM = "pause";
const char string3_50[] PROGMEM = "{ Km/h";
const char string3_51[] PROGMEM = "{ Allure au km";
const char string3_52[] PROGMEM = "{ BPM";
const char string3_53[] PROGMEM = "max BPM";
const char string3_54[] PROGMEM = "max Km/h";
const char string3_55[] PROGMEM = "R$sultat";
const char string3_56[] PROGMEM = "{ Watts";
const char string3_57[] PROGMEM = "R$glez la";
const char string3_58[] PROGMEM = "r$sistance a";
const char string3_59[] PROGMEM = "maximum";
const char string3_60[] PROGMEM = "usqu'a la";
const char string3_61[] PROGMEM = "but$e";
const char string3_62[] PROGMEM = "Au minimum";
const char string3_63[] PROGMEM = "jusqu'a la but$e";
const char string3_64[] PROGMEM = "Calibrage";
const char string3_65[] PROGMEM = "fini";
const char string3_66[] PROGMEM = "$tape";
const char string3_67[] PROGMEM = "Proj.";
const char string3_68[] PROGMEM = "Contraste";
const char string3_69[] PROGMEM = "Élev$";
const char string3_70[] PROGMEM = "Moyen";
const char string3_71[] PROGMEM = "Faible";
const char string3_72[] PROGMEM = "Personn.";
const char string3_73[] PROGMEM = "Config.";
const char string3_74[] PROGMEM = "Batterie";
const char string3_75[] PROGMEM = "R$seau";



const char string4_1[] PROGMEM = "";
const char string4_2[] PROGMEM = "Watios";
#ifdef CICLOTTE
const char string4_3[] PROGMEM = "Marcha";
#else
const char string4_3[] PROGMEM = "Resistencia";
#endif

const char string4_4[] PROGMEM = "RPM";
const char string4_5[] PROGMEM = "Tiempo";
const char string4_6[] PROGMEM = "Distancia";
const char string4_7[] PROGMEM = "Bici libre";
const char string4_8[] PROGMEM = "Ajustes";
const char string4_9[] PROGMEM = "Km/H";
const char string4_10[] PROGMEM = "Watios";
const char string4_11[] PROGMEM = "Distancia/m)";
const char string4_12[] PROGMEM = "minutos";
const char string4_13[] PROGMEM = "comenzar";
const char string4_14[] PROGMEM = "devolver";
const char string4_15[] PROGMEM = "horas";
const char string4_16[] PROGMEM = "PPM";
const char string4_17[] PROGMEM = "Kcal";
const char string4_18[] PROGMEM = "Ritmo/Km";
const char string4_19[] PROGMEM = "Espanol";
const char string4_20[] PROGMEM = "Claro";
const char string4_21[] PROGMEM = "Oscuro";
const char string4_22[] PROGMEM = "Conectar FC";
const char string4_23[] PROGMEM = "Siguiente";
const char string4_24[] PROGMEM = "principal";
const char string4_25[] PROGMEM = "BT apagado";
const char string4_26[] PROGMEM = "BT encendido";
const char string4_27[] PROGMEM = "Informaci@n";
const char string4_28[] PROGMEM = "Calibraci@n";
const char string4_29[] PROGMEM = "los jogos";
const char string4_30[] PROGMEM = "para saber mas";
const char string4_31[] PROGMEM = "Informaci@n";
const char string4_32[] PROGMEM = "Escanear";
const char string4_33[] PROGMEM = "el c@digo";
const char string4_34[] PROGMEM = "QR-Code";
const char string4_35[] PROGMEM = "HR";
const char string4_36[] PROGMEM = "desactivado";
const char string4_37[] PROGMEM = "No se ha";
const char string4_38[] PROGMEM = "encontrado";
const char string4_39[] PROGMEM = "el sensor";
const char string4_40[] PROGMEM = "conectado";
const char string4_41[] PROGMEM = "desconectar";
const char string4_42[] PROGMEM = "conectado";
const char string4_43[] PROGMEM = "con";
const char string4_44[] PROGMEM = "App";
const char string4_45[] PROGMEM = "Fin";
const char string4_46[] PROGMEM = "No";
const char string4_47[] PROGMEM = "Si";
const char string4_48[] PROGMEM = "  Fin?";
const char string4_49[] PROGMEM = "Pausa";
const char string4_50[] PROGMEM = "{ km/h";
const char string4_51[] PROGMEM = "{ Allure au km";
const char string4_52[] PROGMEM = "{ BPM";
const char string4_53[] PROGMEM = "max BPM";
const char string4_54[] PROGMEM = "max km/h";
const char string4_55[] PROGMEM = "resultado";
const char string4_56[] PROGMEM = "{ Watios";
const char string4_57[] PROGMEM = "ajuste";
const char string4_58[] PROGMEM = "la";
const char string4_59[] PROGMEM = "resistencia";
const char string4_60[] PROGMEM = "al maximo";
const char string4_61[] PROGMEM = "para detener";
const char string4_62[] PROGMEM = "sobre Min.";
const char string4_63[] PROGMEM = "para detener";
const char string4_64[] PROGMEM = "Calibraci@n";
const char string4_65[] PROGMEM = "listo";
const char string4_66[] PROGMEM = "Paso";
const char string4_67[] PROGMEM = "Proj.";
const char string4_68[] PROGMEM = "Contraste";
const char string4_69[] PROGMEM = "Alto";
const char string4_70[] PROGMEM = "Medio";
const char string4_71[] PROGMEM = "Bajo";
const char string4_72[] PROGMEM = "Personalizar";
const char string4_73[] PROGMEM = "Configu.";
const char string4_74[] PROGMEM = "Bateria";
const char string4_75[] PROGMEM = "Red";




const char string5_9[] PROGMEM = "mph";
const char string5_11[] PROGMEM = "Distance/mi";
const char string5_18[] PROGMEM = "Pace/mi";
const char string5_19[] PROGMEM = "English(mi)";
const char string5_50[] PROGMEM = "{ mph";

const char* const Translated_Strings_Pointers[][75] PROGMEM = {
	{string1_1, string1_2, string1_3, string1_4, string1_5, string1_6, string1_7, string1_8, string1_9, string1_10, string1_11, string1_12, string1_13, string1_14, string1_15, string1_16, string1_17, string1_18, string1_19, string1_20, string1_21, string1_22, string1_23, string1_24, string1_25, string1_26, string1_27, string1_28, string1_29, string1_30, string1_31, string1_32, string1_33, string1_34, string1_35, string1_36, string1_37, string1_38, string1_39, string1_40, string1_41, string1_42, string1_43, string1_44, string1_45, string1_46, string1_47, string1_48, string1_49, string1_50, string1_51, string1_52, string1_53, string1_54, string1_55, string1_56, string1_57, string1_58, string1_59, string1_60, string1_61, string1_62, string1_63, string1_64, string1_65, string1_66, string1_67, string1_68, string1_69, string1_70, string1_71, string1_72, string1_73, string1_74, string1_75},
	{string2_1, string2_2, string2_3, string2_4, string2_5, string2_6, string2_7, string2_8, string2_9, string2_10, string2_11, string2_12, string2_13, string2_14, string2_15, string2_16, string2_17, string2_18, string2_19, string2_20, string2_21, string2_22, string2_23, string2_24, string2_25, string2_26, string2_27, string2_28, string2_29, string2_30, string2_31, string2_32, string2_33, string2_34, string2_35, string2_36, string2_37, string2_38, string2_39, string2_40, string2_41, string2_42, string2_43, string2_44, string2_45, string2_46, string2_47, string2_48, string2_49, string2_50, string2_51, string2_52, string2_53, string2_54, string2_55, string2_56, string2_57, string2_58, string2_59, string2_60, string2_61, string2_62, string2_63, string2_64, string2_65, string2_66, string2_67, string2_68, string2_69, string2_70, string2_71, string2_72, string2_73, string2_74, string2_75},
	{string3_1, string3_2, string3_3, string3_4, string3_5, string3_6, string3_7, string3_8, string3_9, string3_10, string3_11, string3_12, string3_13, string3_14, string3_15, string3_16, string3_17, string3_18, string3_19, string3_20, string3_21, string3_22, string3_23, string3_24, string3_25, string3_26, string3_27, string3_28, string3_29, string3_30, string3_31, string3_32, string3_33, string3_34, string3_35, string3_36, string3_37, string3_38, string3_39, string3_40, string3_41, string3_42, string3_43, string3_44, string3_45, string3_46, string3_47, string3_48, string3_49, string3_50, string3_51, string3_52, string3_53, string3_54, string3_55, string3_56, string3_57, string3_58, string3_59, string3_60, string3_61, string3_62, string3_63, string3_64, string3_65, string3_66, string3_67, string3_68, string3_69, string3_70, string3_71, string1_72, string1_73, string1_74, string1_75},
	{string4_1, string4_2, string4_3, string4_4, string4_5, string4_6, string4_7, string4_8, string4_9, string4_10, string4_11, string4_12, string4_13, string4_14, string4_15, string4_16, string4_17, string4_18, string4_19, string4_20, string4_21, string4_22, string4_23, string4_24, string4_25, string4_26, string4_27, string4_28, string4_29, string4_30, string4_31, string4_32, string4_33, string4_34, string4_35, string4_36, string4_37, string4_38, string4_39, string4_40, string4_41, string4_42, string4_43, string4_44, string4_45, string4_46, string4_47, string4_48, string4_49, string4_50, string4_51, string4_52, string4_53, string4_54, string4_55, string4_56, string4_57, string4_58, string4_59, string4_60, string4_61, string4_62, string4_63, string4_64, string4_65, string4_66, string4_67, string4_68, string4_69, string4_70, string4_71, string1_72, string1_73, string1_74, string1_75},
	{string1_1, string1_2, string1_3, string1_4, string1_5, string1_6, string1_7, string1_8, string5_9, string1_10, string5_11, string1_12, string1_13, string1_14, string1_15, string1_16, string1_17, string5_18, string5_19, string1_20, string1_21, string1_22, string1_23, string1_24, string1_25, string1_26, string1_27, string1_28, string1_29, string1_30, string1_31, string1_32, string1_33, string1_34, string1_35, string1_36, string1_37, string1_38, string1_39, string1_40, string1_41, string1_42, string1_43, string1_44, string1_45, string1_46, string1_47, string1_48, string1_49, string5_50, string1_51, string1_52, string1_53, string1_54, string1_55, string1_56, string1_57, string1_58, string1_59, string1_60, string1_61, string1_62, string1_63, string1_64, string1_65, string1_66, string1_67, string1_68, string1_69, string1_70, string1_71, string1_72, string1_73, string1_74, string1_75}
};


/*
#ifdef SPRINTBOK
const char* const Translated_Strings_Pointers[][75] PROGMEM = {
	{string1_1, string1_2, string1_3, string1_4, string1_5, string1_6, string1_7, string1_8, string1_9, string1_10, string1_11, string1_12, string1_13, string1_14, string1_15, string1_16, string1_17, string1_18, string1_19, string1_20, string1_21, string1_22, string1_23, string1_24, string1_25, string1_26, string1_27, string1_28, string1_29, string1_30, string1_31, string1_32, string1_33, string1_34, string1_35, string1_36, string1_37, string1_38, string1_39, string1_40, string1_41, string1_42, string1_43, string1_44, string1_45, string1_46, string1_47, string1_48, string1_49, string1_50, string1_51, string1_52, string1_53, string1_54, string1_55, string1_56, string1_57, string1_58, string1_59, string1_60, string1_61, string1_62, string1_63, string1_64, string1_65, string1_66, string1_67, string1_68, string1_69, string1_70, string1_71, string1_72, string1_73, string1_74, string1_75},
	{string2_1, string2_2, string2_3, string2_4, string2_5, string2_6, string2_7, string2_8, string2_9, string2_10, string2_11, string2_12, string2_13, string2_14, string2_15, string2_16, string2_17, string2_18, string2_19, string2_20, string2_21, string2_22, string2_23, string2_24, string2_25, string2_26, string2_27, string2_28, string2_29, string2_30, string2_31, string2_32, string2_33, string2_34, string2_35, string2_36, string2_37, string2_38, string2_39, string2_40, string2_41, string2_42, string2_43, string2_44, string2_45, string2_46, string2_47, string2_48, string2_49, string2_50, string2_51, string2_52, string2_53, string2_54, string2_55, string2_56, string2_57, string2_58, string2_59, string2_60, string2_61, string2_62, string2_63, string2_64, string2_65, string2_66, string2_67, string2_68, string2_69, string2_70, string2_71, string2_72, string2_73, string2_74, string2_75},
	{string3_1, string3_2, string3_3, string3_4, string3_5, string3_6, string3_7, string3_8, string3_9, string3_10, string3_11, string3_12, string3_13, string3_14, string3_15, string3_16, string3_17, string3_18, string3_19, string3_20, string3_21, string3_22, string3_23, string3_24, string3_25, string3_26, string3_27, string3_28, string3_29, string3_30, string3_31, string3_32, string3_33, string3_34, string3_35, string3_36, string3_37, string3_38, string3_39, string3_40, string3_41, string3_42, string3_43, string3_44, string3_45, string3_46, string3_47, string3_48, string3_49, string3_50, string3_51, string3_52, string3_53, string3_54, string3_55, string3_56, string3_57, string3_58, string3_59, string3_60, string3_61, string3_62, string3_63, string3_64, string3_65, string3_66, string3_67, string3_68, string3_69, string3_70, string3_71, string1_72, string1_73, string1_74, string1_75},
	{string4_1, string4_2, string4_3, string4_4, string4_5, string4_6, string4_7, string4_8, string4_9, string4_10, string4_11, string4_12, string4_13, string4_14, string4_15, string4_16, string4_17, string4_18, string4_19, string4_20, string4_21, string4_22, string4_23, string4_24, string4_25, string4_26, string4_27, string4_28, string4_29, string4_30, string4_31, string4_32, string4_33, string4_34, string4_35, string4_36, string4_37, string4_38, string4_39, string4_40, string4_41, string4_42, string4_43, string4_44, string4_45, string4_46, string4_47, string4_48, string4_49, string4_50, string4_51, string4_52, string4_53, string4_54, string4_55, string4_56, string4_57, string4_58, string4_59, string4_60, string4_61, string4_62, string4_63, string4_64, string4_65, string4_66, string4_67, string4_68, string4_69, string4_70, string4_71, string1_72, string1_73, string1_74, string1_75},
	{string1_1, string1_2, string1_3, string1_4, string1_5, string1_6, string1_7, string1_8, string5_9, string1_10, string5_11, string1_12, string1_13, string1_14, string1_15, string1_16, string1_17, string5_18, string5_19, string1_20, string1_21, string1_22, string1_23, string1_24, string1_25, string1_26, string1_27, string1_28, string1_29, string1_30, string1_31, string1_32, string1_33, string1_34, string1_35, string1_36, string1_37, string1_38, string1_39, string1_40, string1_41, string1_42, string1_43, string1_44, string1_45, string1_46, string1_47, string1_48, string1_49, string5_50, string1_51, string1_52, string1_53, string1_54, string1_55, string1_56, string1_57, string1_58, string1_59, string1_60, string1_61, string1_62, string1_63, string1_64, string1_65, string1_66, string1_67, string1_68, string1_69, string1_70, string1_71, string1_72, string1_73, string1_74, string1_75}
};
#else
const char* const Translated_Strings_Pointers[][71] PROGMEM = {
	{string1_1, string1_2, string1_3, string1_4, string1_5, string1_6, string1_7, string1_8, string1_9, string1_10, string1_11, string1_12, string1_13, string1_14, string1_15, string1_16, string1_17, string1_18, string1_19, string1_20, string1_21, string1_22, string1_23, string1_24, string1_25, string1_26, string1_27, string1_28, string1_29, string1_30, string1_31, string1_32, string1_33, string1_34, string1_35, string1_36, string1_37, string1_38, string1_39, string1_40, string1_41, string1_42, string1_43, string1_44, string1_45, string1_46, string1_47, string1_48, string1_49, string1_50, string1_51, string1_52, string1_53, string1_54, string1_55, string1_56, string1_57, string1_58, string1_59, string1_60, string1_61, string1_62, string1_63, string1_64, string1_65, string1_66},
	{string2_1, string2_2, string2_3, string2_4, string2_5, string2_6, string2_7, string2_8, string2_9, string2_10, string2_11, string2_12, string2_13, string2_14, string2_15, string2_16, string2_17, string2_18, string2_19, string2_20, string2_21, string2_22, string2_23, string2_24, string2_25, string2_26, string2_27, string2_28, string2_29, string2_30, string2_31, string2_32, string2_33, string2_34, string2_35, string2_36, string2_37, string2_38, string2_39, string2_40, string2_41, string2_42, string2_43, string2_44, string2_45, string2_46, string2_47, string2_48, string2_49, string2_50, string2_51, string2_52, string2_53, string2_54, string2_55, string2_56, string2_57, string2_58, string2_59, string2_60, string2_61, string2_62, string2_63, string2_64, string2_65, string2_66},
	{string3_1, string3_2, string3_3, string3_4, string3_5, string3_6, string3_7, string3_8, string3_9, string3_10, string3_11, string3_12, string3_13, string3_14, string3_15, string3_16, string3_17, string3_18, string3_19, string3_20, string3_21, string3_22, string3_23, string3_24, string3_25, string3_26, string3_27, string3_28, string3_29, string3_30, string3_31, string3_32, string3_33, string3_34, string3_35, string3_36, string3_37, string3_38, string3_39, string3_40, string3_41, string3_42, string3_43, string3_44, string3_45, string3_46, string3_47, string3_48, string3_49, string3_50, string3_51, string3_52, string3_53, string3_54, string3_55, string3_56, string3_57, string3_58, string3_59, string3_60, string3_61, string3_62, string3_63, string3_64, string3_65, string3_66},
	{string4_1, string4_2, string4_3, string4_4, string4_5, string4_6, string4_7, string4_8, string4_9, string4_10, string4_11, string4_12, string4_13, string4_14, string4_15, string4_16, string4_17, string4_18, string4_19, string4_20, string4_21, string4_22, string4_23, string4_24, string4_25, string4_26, string4_27, string4_28, string4_29, string4_30, string4_31, string4_32, string4_33, string4_34, string4_35, string4_36, string4_37, string4_38, string4_39, string4_40, string4_41, string4_42, string4_43, string4_44, string4_45, string4_46, string4_47, string4_48, string4_49, string4_50, string4_51, string4_52, string4_53, string4_54, string4_55, string4_56, string4_57, string4_58, string4_59, string4_60, string4_61, string4_62, string4_63, string4_64, string4_65, string4_66},
	{string1_1, string1_2, string1_3, string1_4, string1_5, string1_6, string1_7, string1_8, string5_9, string1_10, string5_11, string1_12, string1_13, string1_14, string1_15, string1_16, string1_17, string5_18, string5_19, string1_20, string1_21, string1_22, string1_23, string1_24, string1_25, string1_26, string1_27, string1_28, string1_29, string1_30, string1_31, string1_32, string1_33, string1_34, string1_35, string1_36, string1_37, string1_38, string1_39, string1_40, string1_41, string1_42, string1_43, string1_44, string1_45, string1_46, string1_47, string1_48, string1_49, string5_50, string1_51, string1_52, string1_53, string1_54, string1_55, string1_56, string1_57, string1_58, string1_59, string1_60, string1_61, string1_62, string1_63, string1_64, string1_65, string1_66}
};
#endif
*/