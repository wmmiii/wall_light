export interface Light extends LightInfo {
  address: string;
}

export interface LightInfo {
  name: string;
  effect: Effect;
  /** Hue in the space 0 - 255 */
  h: number;
  /** Saturation in the space 0 - 255 */
  s: number;
  /** Value in the space 0 - 255 */
  v: number;
  cycle: boolean;
}

export enum Effect {
  OFF = -1,
  STEADY = 0,
  BREATHE = 1,
  RAIN = 2,
  GRADIENT = 3,
}
