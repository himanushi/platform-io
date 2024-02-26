#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <M5Unified.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <map>
#include <string>

M5GFX display;
AsyncWebServer server(80);

String html = R"html(
<!doctype html>
<html lang="ja" data-theme="dark">
  <head>
    <meta charset="UTF-8" />
    <meta
      name="viewport"
      content="width=device-width, initial-scale=1.0, user-scalable=no, maximum-scale=1.0, minimum-scale=1.0"
    />
    <title>Easy Setting</title>
    <link rel="stylesheet" href="/pico.min.css" />
    <script type="text/javascript" src="/van.min.js"></script>
    <script type="module">
      const { input, button, div, span, pre, h1, h2, label } = van.tags;

      const Settings = async () => {
        const groups = await fetch("/settings.json").then((res) => res.json());
        const formData = {};

        const onInput = (setting, e) => {
          formData[setting.key] = e.target.value;
        };

        const onSave = () => {
          fetch("/settings", {
            method: "POST",
            headers: {
              "Content-Type": "application/json",
            },
            body: JSON.stringify(formData),
          })
            .then((response) => response.json())
            .then((data) => console.log(data))
            .catch((error) => console.error("Error:", error));
        };

        return div(
          ...groups.map((group) => [
            h2(group.category),
            ...group.settings.map((setting) => [
              label(setting.label),
              input({
                type: setting.type,
                name: setting.name,
                oninput: (e) => onInput(setting, e),
                autocomplete: "off",
              }),
            ]),
          ]),
          button({ onclick: onSave }, "Save"),
        );
      };

      (async () => {
        van.add(document.getElementById("main"), [
          h1("Settings"),
          await Settings(),
        ]);
      })();
    </script>
  </head>
  <body>
    <main id="main" class="container"></main>
  </body>
</html>
)html";

const char *vanjs = R"js(
/**
 * @author vanjs-org/van
 * @url https://github.com/vanjs-org/van/blob/main/public/van-1.2.8.nomodule.min.js
 * @license MIT
 */
{let e,t,l,o,r,n,f,s,a,i,d,u,_,w,h,c,g,v,S,y,V,b,m,x,T,j;f=(n=Object).getPrototypeOf,s=document,i={},d=f(a={isConnected:1}),u=f(f),_=(e,t,l,o)=>(e??(setTimeout(l,o),new Set)).add(t),w=(e,t,o)=>{let r=l;l=t;try{return e(o)}catch(e){return console.error(e),o}finally{l=r}},h=e=>e.filter(e=>e.t?.isConnected),c=t=>r=_(r,t,()=>{for(let e of r)e.l=h(e.l),e.o=h(e.o);r=e},1e3),g={get val(){return l?.add(this),this.i},get oldVal(){return l?.add(this),this.u},set val(l){let o=this;if(l!==o.i){o.i=l;let r=[...o.o=h(o.o)];for(let t of r)V(t.f,t.s,t.t),t.t=e;o.l.length?t=_(t,o,T):o.u=l}}},v=e=>({__proto__:g,i:e,u:e,l:[],o:[]}),S=e=>f(e??0)===g,y=(e,t)=>{let l=new Set,r={f:e},n=o;o=[];let f=w(e,l,t);f=(f??s).nodeType?f:new Text(f);for(let e of l)c(e),e.l.push(r);for(let e of o)e.t=f;return o=n,r.t=f},V=(e,t=v(),l)=>{let r=new Set,n={f:e,s:t};n.t=l??o?.push(n)??a,t.val=w(e,r,t.i);for(let e of r)c(e),e.o.push(n);return t},m=t=>new Proxy((l,...o)=>{let[r,...a]=f(o[0]??0)===d?o:[{},...o],_=t?s.createElementNS(t,l):s.createElement(l);for(let[t,o]of n.entries(r)){let r=l=>l?n.getOwnPropertyDescriptor(l,t)??r(f(l)):e,s=l+","+t,a=i[s]??(i[s]=r(f(_))?.set??0),d=t.startsWith("on")?(e,l)=>{let o=t.slice(2);_.removeEventListener(o,l),_.addEventListener(o,e)}:a?a.bind(_):_.setAttribute.bind(_,t),w=f(o??0);w===u&&(!t.startsWith("on")||o.h)&&(o=V(o),w=g),w===g?y(()=>(d(o.val,o.u),_)):d(o)}return b(_,...a)},{get:(t,l)=>t.bind(e,l)}),x=(e,t)=>t?t!==e&&e.replaceWith(t):e.remove(),T=()=>{let l=[...t].filter(e=>e.i!==e.u);t=e;for(let t of new Set(l.flatMap(e=>e.l=h(e.l))))x(t.t,y(t.f,t.t)),t.t=e;for(let e of l)e.u=e.i},j={add:b=(t,...l)=>{for(let o of l.flat(1/0)){let l=f(o??0),r=l===g?y(()=>o.val):l===u?y(o):o;r!=e&&t.append(r)}return t},_:e=>(e.h=1,e),tags:m(),tagsNS:m,state:v,val:e=>S(e)?e.val:e,oldVal:e=>S(e)?e.oldVal:e,derive:V,hydrate:(e,t)=>x(e,y(t,e))},window.van=j;}
)js";

const char *settingsJson = R"json(
[
  {
    "category": "Wi-Fi",
    "settings": [
      { "value": "", "type": "text", "label": "SSID", "key": "wifi_ssid" },
      {
        "value": "",
        "type": "password",
        "label": "Password",
        "key": "wifi_password"
      }
    ]
  },
  {
    "category": "OpenAI API",
    "settings": [
      {
        "value": "",
        "type": "password",
        "label": "API Key",
        "key": "openai_api_key"
      },
      {
        "value": "gpt-3.5-turbo",
        "label": "Model",
        "type": "select",
        "options": [
          { "label": "GPT-3.5", "value": "gpt-3.5" },
          { "label": "GPT-3.5 Turbo", "value": "gpt-3.5-turbo" }
        ],
        "key": "openai_model"
      }
    ]
  }
]
)json";

std::map<std::string, std::string> settings = {{"ssid", ""}, {"password", ""}};

String getSettingsAsJson() {
  DynamicJsonDocument doc(1024);
  JsonObject obj = doc.to<JsonObject>();

  for (const auto &setting : settings) {
    obj[setting.first] = setting.second;
  }

  String output;
  serializeJson(doc, output);
  return output;
}

void updateSetting(const std::string &key, const std::string &value) {
  settings[key] = value;
}

void setup() {
  M5.begin();
  display.begin();
  Serial.begin(115200);
  WiFi.softAP("M5StackAP", "123456789");
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  M5.Lcd.println(IP);

  if (!SPIFFS.begin()) {
    M5.Lcd.println("SPIFFS Mount Failed");
    return;
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });
  server.on("/van.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/javascript", vanjs);
  });
  server.on("/pico.min.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/pico.min.css", "text/css");
  });
  server.on("/settings.json", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", settingsJson);
  });

  server.on(
      "/settings", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
      [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
         size_t index, size_t total) {
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, (const char *)data);
        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.c_str());
          request->send(400, "application/json", "{\"result\":\"error\"}");
          return;
        }

        JsonObject obj = doc.as<JsonObject>();
        for (JsonPair kv : obj) {
          const char *key = kv.key().c_str();
          const char *value = kv.value().as<const char *>();

          settings[key] = value;
        }

        request->send(200, "application/json", "{\"result\":\"success\"}");

        for (const auto &setting : settings) {
          Serial.print(setting.first.c_str());
          Serial.print(": ");
          Serial.println(setting.second.c_str());
        }
      });

  server.begin();
}

void loop() {}
