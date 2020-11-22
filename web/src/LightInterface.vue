<template>
  <md-card class="light-interface">
    <md-card-header class="name">
      <md-card-header-text>
        <div class="md-title">
          {{light.name}}
        </div>
      </md-card-header-text>
      <md-switch
          color="p-primary"
          :value="light.effect === -1"
          @change="onToggle" />
    </md-card-header>

    <md-card-content class="content">
      <md-field>
        <md-select
            name="effect"
            :value="light.effect"
            @input="onEffect">
          <md-option :value="Effect.OFF">Off</md-option>
          <md-option :value="Effect.STEADY">Steady</md-option>
          <md-option :value="Effect.BREATHE">Breathe</md-option>
          <md-option :value="Effect.RAIN">Rain</md-option>
          <md-option :value="Effect.GRADIENT">Gradient</md-option>
        </md-select>
      </md-field>
      <md-switch
          class="cycle"
          :value="!light.cycle"
          @change="onCycle">
            Cycle color
      </md-switch>
      <color-picker
          variant="persistent"
          :value="this.color.hue()"
          :disabled="light.cycle"
          @input="onColor">
      </color-picker>
    </md-card-content>

  </md-card>
</template>

<script lang="ts">
import Color from 'color';
import Vue from 'vue';
import { MdCard, MdField, MdList, MdMenu, MdSwitch } from 'vue-material/dist/components';
import ColorPicker from '@radial-color-picker/vue-color-picker';
import { Component, Prop } from 'vue-property-decorator';

import { Effect, Light, LightInfo } from './Light';
import { setColor, setCycle, setEffect } from './LightHttp';

Vue.use(MdCard);
Vue.use(MdField);
Vue.use(MdList);
Vue.use(MdMenu);
Vue.use(MdSwitch);

Vue.component('color-picker', ColorPicker);

@Component
export default class LightInterface extends Vue {
  readonly Effect = Effect;

  @Prop({ required: true })
  light!: Light;

  private inFlight = false;
  private next: (() => void) | null = null;

  get color(): Color {
    return Color()
      .hsv(this.light.h * 360 / 255,
          this.light.s * 100 / 255,
          this.light.v * 100 / 255);
  }

  get hex(): string {
    return this.color.hex();
  }

  onToggle(on: boolean): void {
    if (this.inFlight) {
      this.next = () => this.onToggle(on);
      return;
    }

    let result: Promise<LightInfo>
    if (on) {
      result = setEffect(this.light.address, Effect.STEADY);
    } else {
      result = setEffect(this.light.address, Effect.OFF);
    }
    result.then(info => this.$emit('change', info))
          .catch().then(() => this.releaseInFlight());
  }

  onEffect(effect: Effect): void {
    if (this.inFlight) {
      this.next = () => this.onEffect(effect);
      return;
    }

    this.inFlight = true;
    setEffect(this.light.address, effect)
        .then(info => this.$emit('change', info))
        .catch().then(() => this.releaseInFlight());
  }

  onCycle(cycle: boolean): void {
    if (this.inFlight) {
      this.next = () => this.onCycle(cycle);
      return;
    }

    this.inFlight = true;
    if (cycle) {
      setCycle(this.light.address)
          .then(info => this.$emit('change', info))
          .catch().then(() => this.releaseInFlight());
    } else {
      setColor(this.light.address, this.color)
          .then(info => this.$emit('change', info))
          .catch().then(() => this.releaseInFlight());
    }
  }

  onColor(hue: number): void {
    if (this.inFlight) {
      this.next = () => this.onColor(hue);
      return;
    }

    this.inFlight = true;
    setColor(this.light.address, Color().hsv(hue, 100, 0))
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
</script>

<style lang="scss">
@import '~@radial-color-picker/vue-color-picker/dist/vue-color-picker.min.css';

.rcp__well {
  border: none;
  box-shadow: none;
}

.md-input {
  width: 100%;
}
</style>

<style lang="scss" scoped>
.content {
  align-items: start;
  column-gap: 1em;
  display: grid;
  grid-template-columns: 1fr 10em;
  row-gap: 1em;

  .md-field {
    grid-column: 1 / 2;
    grid-row: 1 / 2;
  }

  .md-switch {
    grid-column: 1 / 2;
    grid-row: 2 / 3;
  }

  .rcp {
    grid-column: 2 / 3;
    grid-row: 1 / 3;
    height: 10em;
    width: 10em;
  }
}
</style>