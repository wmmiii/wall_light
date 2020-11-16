<template>
  <div>
    <h1>Light Controller</h1>
    <ul>
      <li v-for="light in lights" :key="light.address">
        {{ light.name }}
      </li>
    </ul>
  </div>
</template>

<script lang="ts">
import { Component } from 'vue-property-decorator';
import Vue from 'vue';

import { Light } from './Light';
import { scanLocalNetwork } from './Scanner';

@Component
export default class Index extends Vue {
  lights: Light[] = [];

  mounted() {
    console.log("mounted");
    scanLocalNetwork().forEach(p => p.then(res => {
      console.log(res);
      if (res != null) {
        this.lights.push(res);
      }
    }));
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

</style>
