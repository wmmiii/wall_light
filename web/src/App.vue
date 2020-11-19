<template>
  <div class="app">
    <h1>Light Controller</h1>
    <template v-if="secure">
      <p>
        This controller must be used on an insecure, local connection.
        Try accessing this webpage via the IP address of a LED controller.
      </p>
      <p class="quiet">
        The LED controllers do not support TLS. To prevent mixed-content browser
        errors both this page and the LED controller APIs must be accessed via
        HTTP.
      </p>
    </template>
    <template v-else>
      <p>{{ status }}</p>
      <button :disabled="!scanComplete" @click="scan">Rescan</button>
      <ul>
        <li v-for="light in lights" :key="light.address">
          <light-interface :light="light" @change="info => update(light, info)" />
        </li>
      </ul>
    </template>
  </div>
</template>

<script lang="ts">
import { Component } from 'vue-property-decorator';
import Vue from 'vue';

import { Light, LightInfo } from './Light';
import { scanLocalNetwork } from './LightHttp';
import './LightInterface';

@Component
export default class Index extends Vue {
  lights: Light[] = [];
  scanComplete = false;

  get status(): string {
    if (this.scanComplete && this.lights.length == 0) {
      return "No devices found.";
    } else if (this.scanComplete) {
      return "Scan complete.";
    } else {
      return "Scanning...";
    }
  }

  get secure(): boolean {
    return window.location.protocol === 'https:';
  }

  mounted() {
    this.scan();
  }

  scan() {
    this.scanComplete = false;
    this.lights = [];
    const scan = scanLocalNetwork();
    Promise.all(scan)
        .then(() => this.scanComplete = true)
        .catch(() => this.scanComplete = true);
    scan.forEach(p => p.then(res => {
      if (res != null) {
        this.lights.push(res);
      }
    }));
  }

  update(light: Light, info: LightInfo) {
    const newLight = Object.assign({}, light, info);
    const index = this.lights.indexOf(light);
    this.lights.splice(index, 1, newLight);
  }
}
</script>

<style lang="scss">

html {
  background-color: #222;
  color: #fff;
}

body {
  font-family: Sans-Serif;
  margin: auto;
  max-width: 30em;
}

h1 {
  text-align: center;
}

.app {
  ul {
    list-style: none;
    margin: 0;
    padding: 0;
  }

  li {
    margin-bottom: 1em;
  }

  .quiet {
    color: #aaa;
  }
}

</style>
