<script lang="ts">
  let isOn = false;
  async function onclick(on_off: boolean) {
    isOn = on_off;
    await fetch("/api/toggle-led", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({
        is_on: isOn,
      }),
    });
  }
</script>

{#if isOn}
  <div on:click={() => onclick(false)} class="rounded-full border-4 border-black border-solid w-16 h-16 cursor-pointer bg-blue-700 " />
{:else}
  <div on:click={() => onclick(true)} class="rounded-full border-4 border-black border-solid w-16 h-16 cursor-pointer" />
{/if}
