#ifndef DECLARE_MACROS_H__
#define DECLARE_MACROS_H__

#define DECLARE_INSTANCE(type, inst)		extern type inst
#define DEFINE_INSTANCE(type, inst, __VAR_ARGS__)			type inst(...)

#define DEFINE_WIFI_STATION_INSTANCE(t)		DECLARE_INSTANCE(WiFi_Station, t)


#endif /* DECLARE_MACROS_H__ */
