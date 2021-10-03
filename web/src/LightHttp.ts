import Color from 'color';
import { Effect, Light, LightInfo } from './Light';

export function scanLocalNetwork(): Array<Promise<Light | null>> {
  const lightsPromise: Array<Promise<Light | null>> = [];
  for (let i = 0; i < 255; i++) {
    const address = `http://192.168.0.${i}`;
    const abort = new AbortController();
    setTimeout(() => abort.abort(), 5000);
    const query =  fetch(`${address}/lightInfo`, {
      method: 'GET',
      mode: 'cors',
      cache: 'no-cache',
      signal: abort.signal,
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

export async function setEffect(address: string, effect: Effect): Promise<LightInfo> {
  return fetch(`${address}/effect/${effect}`, {
    method: 'PUT',
    mode: 'cors',
    cache: 'no-cache',
  }).then(res => {
    const body = res.json();
    if (res.ok && body != null) {
      return body;
    } else {
      throw Error('Set effect did not succeed!');
    }
  });
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
    } else {
      throw Error('Set cycle did not succeed!');
    }
  });
}

export async function setColor(address: string, color: Color): Promise<LightInfo> {
  const h = Math.floor(color.hue() * 255 / 360);
  const s = Math.floor(color.saturationv() * 255 / 100);
  const v = 255;
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