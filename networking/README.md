# networking/

`cacert.pem` is the Mozilla CA certificate bundle as extracted by the curl project
(https://curl.se/docs/caextract.html, MPL-2.0). It is copied next to the Starship
executable at build time and used by the Archipelago client to validate `wss://`
connections on platforms whose OpenSSL does not use the system trust store
(Windows, macOS).

To refresh: `curl -sSfL https://curl.se/ca/cacert.pem -o networking/cacert.pem`
