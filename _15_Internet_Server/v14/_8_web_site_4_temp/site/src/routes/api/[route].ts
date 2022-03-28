export async function get({ params }) {
  console.log(params);
  const proxyUrl = `http://my-esp32.local/api/${params.route}`;
  const response = await fetch(proxyUrl);
  const body = await response.json();
  return {
    body,
  };
}

export async function post({ params, request }) {
  const body = await request.json();
  console.log("post hit", params, body);
  const proxyUrl = `http://my-esp32.local/api/${params.route}`;
  const response = await fetch(proxyUrl, { method: "POST", body: JSON.stringify(body) });
  if (response.status !== 200) {
    return {
      status: response.status,
    };
  }
  let body = await response.json();
  return {
    body,
  };
}
