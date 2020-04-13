#ifndef WLAN_H_
#define WLAN_H_

#ifndef WLAN_SSID
#error WLAN_SSID not defined
#endif

#ifndef WLAN_PASSWORD
#error WLAN_PASSWORD not defined
#endif

#ifndef WLAN_CONNECTION_TIMEOUT
#define WLAN_CONNECTION_TIMEOUT 30000
#endif

bool wlan_setup();
bool wlan_ensure_connected();

#endif