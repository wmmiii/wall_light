<template>
  <div class="light-interface">
    <h1>{{ light.name }}</h1>
    <label>
      <input
          type="checkbox"
          :value="light.cycle"
          @input="onCycle" />
          Cycle color
    </label>
    <color-picker
        :startColor="hex"
        :value="hex"
        :disabled="light.cycle"
        @color-change="onColor">
    </color-picker>
  </div>
</template>

<script lang="ts">
import Color from 'color';
import Vue from 'vue';
import ColorPicker from 'vue-color-picker-wheel';
import { Component, Prop } from 'vue-property-decorator';

import { Light } from './Light';
import { setColor, setCycle } from './LightHttp';

@Component
export default class LightInterface extends Vue {
  @Prop({ required: true })
  light!: Light;

  private inFlight = false;
  private next: (() => void) | null = null;

  get color(): Color {
    return Color()
      .hsv(this.light.h, this.light.s, this.light.v);
  }

  get hex(): string {
    return this.color.hex();
  }

  onCycle(event: InputEvent): void {
    if (this.inFlight) {
      this.next = () => this.onCycle(event);
      return;
    }

    this.inFlight = true;
    if ((event.target as HTMLInputElement).checked) {
      setCycle(this.light.address)
          .then(info => this.$emit('change', info))
          .catch().then(() => this.releaseInFlight());
    } else {
      setColor(this.light.address, this.color)
          .then(info => this.$emit('change', info))
          .catch().then(() => this.releaseInFlight());
    }
  }

  onColor(hex: string): void {
    if (this.inFlight) {
      this.next = () => this.onColor(hex);
      return;
    }

    this.inFlight = true;
    setColor(this.light.address, Color(hex))
        .then(info => this.$emit('change', info))
        .catch().then(() => this.releaseInFlight());
  }

  private releaseInFlight(): void {
    this.inFlight = false;
    if (this.next != null) {
      this.next();
      this.next = null;
    }
  }
}

Vue.component('light-interface', LightInterface);
Vue.component('color-picker', ColorPicker);
</script>

<style lang="scss">

.light-interface {
  background-color: #444;
  border-radius: 1em;
  display: block;
  padding: 1em;
}

</style>
