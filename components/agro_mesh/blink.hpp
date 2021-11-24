#ifndef AGRO_MESH_BLINK_DEF_HPP__
#define AGRO_MESH_BLINK_DEF_HPP__

#define BLINK_MS_NOT_CONNECTED		300
#define BLINK_MS_CONNECTED			2000

#ifdef CONFIG_ENABLE_HEARTBEAT_TASK
#define BLINK_INTERVAL_EXTERN		extern int interval_blink;
#define SET_BLINK_MS(x)				interval_blink = x
#else
#define BLINK_INTERVAL_EXTERN
#define SET_BLINK_MS(x)				do{}while(0)
#endif /* CONFIG_ENABLE_HEARTBEAT_TASK */

#endif /* AGRO_MESH_BLINK_DEF_HPP__ */
