<script lang="ts">
  import { onMount } from "svelte";

  let aps = [];
  let ssid = "";
  let password = "";
  const getAps = async () => {
    const res = await fetch("/api/get-ap-list");
    aps = await res.json();
    console.log(aps);
  };

  onMount(getAps);

  const joinNetwork = async () => {
    try {
      const result = await fetch("/api/ap-sta", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ ssid, password }),
      });
      if (result.ok) {
        console.log(result);
        alert("neetwork changed");
        document.location.href = "/";
      }
    } catch (err) {
      alert(err);
    }
  };
</script>

{#if aps.length == 0}
  <svg class="animate-spin -ml-1 mr-3 h-5 w-5 " xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24">
    <circle class="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" stroke-width="4" />
    <path class="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z" />
  </svg>
{:else}
  <select bind:value={ssid} class="select select-bordered w-full max-w-xs">
    <option disabled selected>Who shot first?</option>
    {#each aps as ap}
      <option>{ap.ssid}</option>
    {/each}
  </select>
{/if}
{#if !!ssid}
  <input bind:value={password} type="password" placeholder="password" class="input input-bordered w-full max-w-xs" />
  <button on:click={joinNetwork} class="btn m-3">Submit</button>
{/if}
{ssid}
{password}
