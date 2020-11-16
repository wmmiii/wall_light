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