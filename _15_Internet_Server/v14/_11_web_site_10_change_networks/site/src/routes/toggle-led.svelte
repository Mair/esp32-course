<script lang="ts">
  let isOn = false;

  async function onClick(on_off: boolean) {
    isOn = on_off;
    await fetch("/api/toggle-led", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ is_on: isOn }),
    });
  }
</script>

{#if isOn}
  <div on:click={() => onClick(false)} class="rounded-full border-4 border-black border-solid w-16 h-16 bg-blue-700 cursor-pointer" />
{:else}
  <div on:click={() => onClick(true)} class="rounded-full border-4 border-black border-solid w-16 h-16  cursor-pointer" />
{/if}
