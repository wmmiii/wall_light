export interface Light extends LightInfo {
  address: string;
}

export interface LightInfo {
  name: string;
  effect: number;
  h: number;
  s: number;
  v: number;
  cycle: boolean;
}
