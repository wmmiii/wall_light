<template>
  <div class="app">
    <link rel="stylesheet" href="//fonts.googleapis.com/css?family=Roboto:400,500,700,400italic|Material+Icons">
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
      <div class="status">
        <md-button :disabled="!scanComplete" @click="scan">
          {{ scanComplete ? 'Rescan' : 'Scanning' }}
        </md-button>
        <md-progress-bar v-if="!scanComplete" md-mode="indeterminate"></md-progress-bar>
      </div>
      <div class="layout">
        <light-interface
            class="light"
            v-for="light in lights"
            :key="light.address"
            :light="light"
            @change="info => update(light, info)" />
      </div>
    </template>
  </div>
</template>

<script lang="ts">
import { Component } from 'vue-property-decorator';
import Vue from 'vue';

import 'vue-material/dist/vue-material.min.css';
import 'vue-material/dist/theme/default-dark.css';
import { MdButton, MdProgress } from 'vue-material/dist/components';

import { Light, LightInfo } from './Light';
import { scanLocalNetwork } from './LightHttp';
import './LightInterface';

Vue.use(MdButton);
Vue.use(MdProgress);

@Component
export default class Index extends Vue {
  lights: Light[] = [];
  scanComplete = false;

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
html,
body {
  height: 100%;
  margin: 0;
  padding: 0;
  width: 100%;
}

h1 {
  text-align: center;
}

.app {
  bottom: 0;
  left: 0;
  position: absolute;
  right: 0;
  top: 0;

  display: flex;
  flex-direction: column;

  > * {
    flex-shrink: 0;
  }

  .quiet {
    color: #aaa;
  }

  .status {
    display: flex;
    flex-direction: row;
    align-items: center;

    .md-button {
      flex-shrink: 0;
    }

    .md-progress-bar {
      flex: 1;
      margin-right: 2em;
    }
  }

  .layout {
    align-content: start;
    display: grid;
    flex: 1;
    gap: 1em;
    grid-template-columns: repeat(auto-fill, minmax(m#{i}n(30em, 100%), 1fr));
    justify-content: start;
    overflow-y: auto;
    padding: 1em;
  }

  .light {
    max-width: 45em;
  }
}
</style>
