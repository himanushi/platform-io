#include <M5Unified.h>
#include <WebServer.h>
#include <WiFi.h>

M5GFX display;
WebServer server(80);

String html = R"html(
<!doctype html>
<html lang="ja">
  <head>
    <meta charset="UTF-8" />
    <meta
      name="viewport"
      content="width=device-width, initial-scale=1.0, user-scalable=no, maximum-scale=1.0, minimum-scale=1.0"
    />
    <title>M5Stack</title>
    <script src="/index.js" type="module"></script>
  </head>
  <body></body>
</html>
)html";

const char *vanjs = R"js(
/**
 * @author vanjs-org/van
 * @url https://github.com/vanjs-org/van/blob/main/public/van-1.2.8.min.js
 * @license MIT
 */
let e,t,l,o,r,n=Object,f=n.getPrototypeOf,s=document,a={isConnected:1},i={},d=f(a),u=f(f),_=(e,t,l,o)=>(e??(setTimeout(l,o),new Set)).add(t),h=(e,t,o)=>{let r=l;l=t;try{return e(o)}catch(e){return console.error(e),o}finally{l=r}},c=e=>e.filter(e=>e.t?.isConnected),g=t=>r=_(r,t,()=>{for(let e of r)e.l=c(e.l),e.o=c(e.o);r=e},1e3),w={get val(){return l?.add(this),this.i},get oldVal(){return l?.add(this),this.u},set val(l){let o=this;if(l!==o.i){o.i=l;let r=[...o.o=c(o.o)];for(let t of r)x(t.f,t.s,t.t),t.t=e;o.l.length?t=_(t,o,p):o.u=l}}},v=e=>({__proto__:w,i:e,u:e,l:[],o:[]}),S=e=>f(e??0)===w,y=(e,t)=>{let l=new Set,r={f:e},n=o;o=[];let f=h(e,l,t);f=(f??s).nodeType?f:new Text(f);for(let e of l)g(e),e.l.push(r);for(let e of o)e.t=f;return o=n,r.t=f},x=(e,t=v(),l)=>{let r=new Set,n={f:e,s:t};n.t=l??o?.push(n)??a,t.val=h(e,r,t.i);for(let e of r)g(e),e.o.push(n);return t},V=(t,...l)=>{for(let o of l.flat(1/0)){let l=f(o??0),r=l===w?y(()=>o.val):l===u?y(o):o;r!=e&&t.append(r)}return t},b=t=>new Proxy((l,...o)=>{let[r,...a]=f(o[0]??0)===d?o:[{},...o],_=t?s.createElementNS(t,l):s.createElement(l);for(let[t,o]of n.entries(r)){let r=l=>l?n.getOwnPropertyDescriptor(l,t)??r(f(l)):e,s=l+","+t,a=i[s]??(i[s]=r(f(_))?.set??0),d=t.startsWith("on")?(e,l)=>{let o=t.slice(2);_.removeEventListener(o,l),_.addEventListener(o,e)}:a?a.bind(_):_.setAttribute.bind(_,t),h=f(o??0);h===u&&(!t.startsWith("on")||o.h)&&(o=x(o),h=w),h===w?y(()=>(d(o.val,o.u),_)):d(o)}return V(_,...a)},{get:(t,l)=>t.bind(e,l)}),m=(e,t)=>t?t!==e&&e.replaceWith(t):e.remove(),p=()=>{let l=[...t].filter(e=>e.i!==e.u);t=e;for(let t of new Set(l.flatMap(e=>e.l=c(e.l))))m(t.t,y(t.f,t.t)),t.t=e;for(let e of l)e.u=e.i};export default{add:V,_:e=>(e.h=1,e),tags:b(),tagsNS:b,state:v,val:e=>S(e)?e.val:e,oldVal:e=>S(e)?e.oldVal:e,derive:x,hydrate:(e,t)=>m(e,y(t,e))};
)js";

const char *js = R"js(
import van from "/van.min.js";
)js";

const char *settingsJson = R"json(
{"settings": [
  {"value": null, type: "text", label: "SSID", id: "ssid"},
  {"value": null, type: "text", label: "Password", id: "ssid"},
]}
)json";

void setup() {
  M5.begin();
  display.begin();
  Serial.begin(115200);
  WiFi.softAP("M5StackAP", "123456789");
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, []() { server.send(200, "text/html", html); });
  server.on("/van.min.js", HTTP_GET,
            []() { server.send(200, "application/javascript", vanjs); });
  server.on("/index.js", HTTP_GET,
            []() { server.send(200, "application/javascript", js); });
  server.on("/settings.json", HTTP_GET,
            []() { server.send(200, "application/json", settingsJson); });

  server.begin();
}

void loop() { server.handleClient(); }
