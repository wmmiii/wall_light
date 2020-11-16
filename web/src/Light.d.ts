export interface Light extends LightInfo {
  address: String;
}

export interface LightInfo {
  name: String;
  effect: number;
  h: number;
  s: number;
  v: number;
  cycle: boolean;
}
