<script lang="ts">
  import { onMount } from "svelte";

  let count = 0;
  let button_state = "released";
  onMount(() => {
    var socket = new WebSocket("ws://my-esp32/ws");
    socket.onopen = function (event) {
      socket.send("A connection has been established!");
    };
    socket.onmessage = function (event) {
      const data = JSON.parse(event.data);
      button_state = !data.btn_state ? "pressed" : "released";
      if (count++ > 99) count = 0;
    };
  });
</script>

<span class="countdown font-mono text-9xl">
  <span style={`--value:${count};`} />
</span>

<div class="alert alert-info shadow-lg w-32">
  <div>
    <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" class="stroke-current flex-shrink-0 w-6 h-6"
      ><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M13 16h-1v-4h-1m1-4h.01M21 12a9 9 0 11-18 0 9 9 0 0118 0z" /></svg
    >
    <span>{button_state}</span>
  </div>
</div>
