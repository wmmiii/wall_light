export interface Light extends LightInfo {
  address: string;
}

export interface LightInfo {
  name: string;
  effect: number;
  /** Hue in the space 0 - 255 */
  h: number;
  /** Saturation in the space 0 - 255 */
  s: number;
  /** Value in the space 0 - 255 */
  v: number;
  cycle: boolean;
}
