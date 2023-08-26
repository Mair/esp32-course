```c
  if (!esp_now_is_peer_exist(payload_ext.from_mac))
      {
        ESP_LOGI(TAG, "got broadcast from %s", mac_to_str(buffer, payload_ext.from_mac));
        esp_now_peer_info_t peer;
        memset(&peer, 0, sizeof(esp_now_peer_info_t));
        memcpy(peer.peer_addr, payload_ext.from_mac, 6);
        memcpy(peer.lmk, "thisIsMyKeyEnc1", 16);
        peer.encrypt = true;
        esp_now_add_peer(&peer);
      }
```
