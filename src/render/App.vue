<template>
    <h1>Application Monitor</h1>
    <!-- <h3>Window Status of Finder: Recents</h3>
    <p>
      Status: {{ windowStatus.status }}
      <br>
      Dimensions: 
      <br>
      x: {{ windowStatus.dimensions.x }}
      <br>
      y: {{ windowStatus.dimensions.y }}
      <br>
      height: {{ windowStatus.dimensions.height }}
      <br>
      width: {{ windowStatus.dimensions.width }}
      <br>
    </p> -->
    <p>
      <template v-if="!hasReceivedApplications">
        Waiting for application scanner to return results
      </template>
      <template v-else-if="!hasScannedApplications">
        No scanned applications received
      </template>
      <template v-else>
        Active applications with windows: {{ scannedApplications.length }}
        <ul>
          <li v-for="app in scannedApplications">
            Application: {{ app.procedureName }}
            <br>
            Window Title: {{ app.windowTitle }}
          </li>
        </ul>
      </template>
    </p>
  </template>
  
<script setup lang="ts">
import { computed, onMounted, ref } from 'vue';

const hasReceivedApplications = ref(false);
const scannedApplications = ref<ScannedApplications>([]);
const hasScannedApplications = computed(() => {
  return scannedApplications.value.length > 0;
});
const windowStatus = ref<WindowStatus>();

onMounted(() => {
  window.ApplicationMonitorApi.StartApplicationScanner((newScannedApplications) => {
    hasReceivedApplications.value = true;
    scannedApplications.value = newScannedApplications;
  });

  window.ApplicationMonitorApi.StartApplicationStatus('Finder', 'Recents', (newWindowStatus) => {
      windowStatus.value = newWindowStatus;
    });
});
</script>

<style lang="css" scoped>
li {
  line-height: 1.5rem;
}
li + li {
  padding-top: 1rem;
}
</style>