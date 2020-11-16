import Color from 'color';
import { Light, LightInfo } from './Light';

export function scanLocalNetwork(): Array<Promise<Light | null>> {
  const lightsPromise: Array<Promise<Light | null>> = [];
  for (let i = 208; i < 209; i++) {
    const address = `http://10.0.0.${i}`;
    const query =  fetch(`${address}/lightInfo`, {
      method: 'GET',
      mode: 'cors',
      cache: 'no-cache',
    })
    .then(res => {
      if (res.ok) {
        return res.json();
      } else {
        return null;
      }
    })
    .then((i: LightInfo) => {
      return {
        address: address,
        ...i,
      } as Light;
    })
    .catch(() => null);
    lightsPromise.push(query);
  }

  return lightsPromise;
}

export async function setCycle(address: string): Promise<LightInfo> {
  return fetch(`${address}/cycle`, {
    method: 'PUT',
    mode: 'cors',
    cache: 'no-cache',
  }).then(res => {
    const body = res.json();
    if (res.ok && body != null) {
      return body;
      return res.json();
    } else {
      throw Error('Set cycle did not succeed!');
    }
  });
}

export async function setColor(address: string, color: Color): Promise<LightInfo> {
  const h = Math.floor(color.hue());
  const s = Math.floor(color.saturationv());
  const v = Math.floor(color.value());
  return fetch(`${address}/color/${h}-${s}-${v}`, {
    method: 'PUT',
    mode: 'cors',
    cache: 'no-cache',
  }).then(res => {
    const body = res.json();
    if (res.ok && body != null) {
      return body;
    } else {
      throw Error('Set color did not succeed!');
    }
  });
}