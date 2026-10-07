#pragma once
#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8"/>
<meta name="viewport" content="width=device-width,initial-scale=1"/>
<title>Swarm Bot Controller</title>
<style>
:root{
  --bg:#070b0e;--panel:#0d1319;--card:#121a22;--border:#1f2b37;
  --txt:#f1f5f9;--muted:#64748b;--accent:#38bdf8;--green:#10b981;
  --amber:#f59e0b;--red:#ef4444;
}
*{box-sizing:border-box;margin:0;padding:0}
body{background:var(--bg);color:var(--txt);font-family:system-ui,-apple-system,sans-serif;padding:16px}
.header{display:flex;justify-content:space-between;align-items:center;padding:12px 18px;background:var(--panel);border:1px solid var(--border);border-radius:14px;margin-bottom:16px}
.brand{display:flex;align-items:center;gap:12px}
.brand h1{font-size:17px;font-weight:800;letter-spacing:.08em;color:#fff}
.badges{display:flex;gap:8px;flex-wrap:wrap}
.badge{font-size:11px;font-weight:700;padding:4px 9px;border-radius:6px;background:#1e293b;color:#94a3b8;font-family:ui-monospace,monospace}
.badge.ok{background:#064e3b;color:#6ee7b7}
.badge.warn{background:#78350f;color:#fde68a}
.badge.locked{background:#831843;color:#fbcfe8}

.layout{display:grid;grid-template-columns:1fr 340px;gap:16px}
@media(max-width:850px){.layout{grid-template-columns:1fr}}

.card{background:var(--panel);border:1px solid var(--border);border-radius:14px;padding:16px;margin-bottom:16px}
.card h2{font-size:13px;font-weight:700;color:#94a3b8;letter-spacing:.06em;margin-bottom:12px;display:flex;justify-content:space-between;align-items:center}
.tabs{display:flex;gap:6px;border-bottom:1px solid var(--border);padding-bottom:10px;margin-bottom:14px;overflow-x:auto}
.tabbtn{background:transparent;color:var(--muted);border:none;padding:6px 12px;border-radius:6px;cursor:pointer;font-size:12px;font-weight:700}
.tabbtn.active{background:#1e293b;color:#fff}

.grid-2{display:grid;grid-template-columns:1fr 1fr;gap:12px}
@media(max-width:550px){.grid-2{grid-template-columns:1fr}}
.form-group{margin-bottom:10px}
.form-group label{display:block;font-size:11px;font-weight:600;color:var(--muted);margin-bottom:4px}
.form-group input,.form-group select{width:100%;background:#090d12;color:#fff;border:1px solid var(--border);border-radius:8px;padding:8px 10px;font-size:12px;font-family:ui-monospace,monospace}

.btn{background:#16202a;color:var(--txt);border:1px solid var(--border);border-radius:8px;padding:9px 12px;font-size:12px;font-weight:700;cursor:pointer;text-align:center;transition:background .1s,transform .05s}
.btn:hover{background:#22303e}
.btn:active{transform:scale(0.97)}
.btn.primary{background:#0284c7;border-color:#38bdf8;color:#fff}
.btn.danger{background:#dc2626;border-color:#ef4444;color:#fff}

.oled-frame{background:linear-gradient(#05253b,#031826);border:1px solid #134e70;border-radius:14px;padding:14px;box-shadow:inset 0 0 0 2px #021624,0 12px 30px #000a}
.oled-pins{display:flex;justify-content:center;gap:18px;font:700 10px monospace;color:#bae6fd;margin-bottom:6px;opacity:.9}
.screen-housing{background:#000;border:4px solid #1a2228;border-radius:6px;padding:3px;box-shadow:inset 0 0 15px #000,0 0 0 1px #475569}
canvas#twinCanvas{display:block;width:100%;aspect-ratio:2/1;background:#000;image-rendering:pixelated;image-rendering:crisp-edges}

.btn-grid{display:grid;grid-template-columns:repeat(3,1fr);gap:6px;margin-top:12px}

.tactile-wrap{display:flex;flex-direction:column;align-items:center;margin-top:14px;gap:10px;background:rgba(11,16,22,.8);border:1px solid var(--border);border-radius:14px;padding:14px}
.btn-housing{position:relative;width:116px;height:116px;border-radius:50%;background:radial-gradient(circle at 50% 50%,#1e293b,#090d12 70%);border:3px solid #334155;box-shadow:inset 0 2px 5px rgba(255,255,255,.1),0 8px 20px rgba(0,0,0,.7);display:flex;align-items:center;justify-content:center;transition:border-color .12s,box-shadow .12s}
.btn-housing.active-select{border-color:#10b981;box-shadow:0 0 22px rgba(16,185,129,.6),inset 0 0 12px rgba(16,185,129,.3)}
.btn-housing.active-power{border-color:#ef4444;box-shadow:0 0 28px rgba(239,68,68,.7),inset 0 0 16px rgba(239,68,68,.4)}

.bigbtn{width:90px;height:90px;border-radius:50%;border:4px solid #475569;background:radial-gradient(circle at 40% 35%,#64748b,#1e293b 45%,#0f172a 75%);box-shadow:0 7px 0 #020617,0 10px 18px rgba(0,0,0,.6);cursor:pointer;touch-action:none;user-select:none;transition:transform .04s,box-shadow .04s;display:flex;flex-direction:column;align-items:center;justify-content:center;color:#f8fafc}
.bigbtn:hover{background:radial-gradient(circle at 40% 35%,#94a3b8,#334155 45%,#0f172a 75%)}
.bigbtn.down{transform:translateY(6px);box-shadow:0 1px 0 #020617,0 3px 8px rgba(0,0,0,.8);border-color:#38bdf8}
.btn-cap-icon{font-size:20px;line-height:1}
.btn-cap-label{font-size:10px;font-weight:800;letter-spacing:.06em;color:#cbd5e1;margin-top:2px}

.hold-meter{width:100%;display:flex;flex-direction:column;gap:5px}
.hold-track{width:100%;height:8px;background:#090d12;border:1px solid #334155;border-radius:999px;overflow:hidden}
.hold-bar{height:100%;width:0%;background:linear-gradient(90deg,#38bdf8,#10b981 70%,#ef4444 100%);border-radius:999px;transition:width .03s linear}
.hold-info{display:flex;justify-content:space-between;align-items:center;font-family:ui-monospace,monospace;font-size:11px}
.hold-status{font-weight:700;padding:2px 7px;border-radius:4px;background:#1e293b;color:#94a3b8}
.hold-status.select{background:#064e3b;color:#6ee7b7}
.hold-status.power{background:#7f1d1d;color:#fca5a5}

.quick-chips{display:grid;grid-template-columns:1fr 1fr;gap:5px;width:100%;margin-top:4px}
.chip-btn{background:#131c26;border:1px solid #334155;border-radius:7px;padding:6px 8px;font-size:10px;font-weight:600;color:var(--txt);cursor:pointer;text-align:center;transition:background .1s,border-color .1s}
.chip-btn:hover{background:#1e293b;border-color:#38bdf8}
.chip-btn:active{transform:scale(0.97)}

.telemetry{font-family:ui-monospace,monospace;font-size:11px;background:#0b1016;border:1px solid var(--border);border-radius:8px;padding:9px;line-height:1.7}
.tag-live{color:var(--green);font-weight:700}
.tag-mock{color:var(--amber);font-weight:700}
.alert{padding:9px 12px;border-radius:8px;margin-bottom:12px;font-size:12px;display:none}
.alert.error{background:#450a0a;border:1px solid #dc2626;color:#fca5a5;display:block}
.alert.success{background:#052e16;border:1px solid #16a34a;color:#86efac;display:block}
</style>
</head>
<body>
<div class="header">
  <div class="brand">
    <h1>SWARM BOT</h1>
    <span class="badge" id="botBadge">BOT 01</span>
  </div>
  <div class="badges">
    <span class="badge ok" id="netBadge">CONNECTED</span>
    <span class="badge ok" id="batBadge">82%</span>
    <span class="badge" id="modeBadge">FOLLOW</span>
    <span class="badge locked" id="uiBadge">UI ACTIVE</span>
  </div>
</div>

<div class="layout">
  <div class="left-col">
    <div class="card">
      <div class="tabs">
        <button class="tabbtn active" onclick="switchTab('status')">Telemetry</button>
        <button class="tabbtn" onclick="switchTab('gpio')">Hardware & GPIO</button>
        <button class="tabbtn" onclick="switchTab('modes')">Modes</button>
        <button class="tabbtn" onclick="switchTab('system')">System</button>
      </div>

      <div id="tab-status">
        <div class="grid-2">
          <div class="telemetry">
            <div>Bot ID: <b id="tBotId">1</b></div>
            <div>Mode: <b id="tMode">FOLLOW</b></div>
            <div>Page: <b id="tPage">HOME</b></div>
            <div>UI Motion Lock: <b id="tUiLock">ACTIVE</b></div>
          </div>
          <div class="telemetry">
            <div>Battery: <b id="tBat">82%</b> <span id="tBatTag" class="tag-live">[LIVE]</span></div>
            <div>Voltage: <b id="tVolt">3.95V</b></div>
            <div>Swarm Net: <b id="tNet">CONNECTED</b></div>
            <div>Connected Bots: <b id="tBots">4 / 4</b></div>
          </div>
        </div>
      </div>

      <div id="tab-gpio" style="display:none">
        <div id="cfgAlert" class="alert"></div>
        <form id="gpioForm" onsubmit="saveConfig(event)">
          <div class="grid-2">
            <div class="form-group"><label>Bot ID (1-9):</label><input type="number" id="cfgBotId" min="1" max="9" value="1" required/></div>
            <div class="form-group"><label>OLED Driver:</label><select id="cfgDriver"><option value="0">SH1106 (128x64)</option><option value="1">SSD1306 (128x64)</option></select></div>
            <div class="form-group"><label>OLED I2C Address:</label><input type="text" id="cfgAddr" value="0x3C" required/></div>
            <div class="form-group"><label>OLED SDA GPIO:</label><input type="number" id="cfgSda" value="21" required/></div>
            <div class="form-group"><label>OLED SCL GPIO:</label><input type="number" id="cfgScl" value="22" required/></div>
            <div class="form-group"><label>Input Mode:</label><select id="cfgInMode"><option value="0">Single Button</option><option value="1">Multi Button</option></select></div>
            <div class="form-group"><label>Single Button GPIO:</label><input type="number" id="cfgBtnSingle" value="27" required/></div>
            <div class="form-group"><label>Prev Button GPIO:</label><input type="number" id="cfgBtnPrev" value="32"/></div>
            <div class="form-group"><label>Next Button GPIO:</label><input type="number" id="cfgBtnNext" value="33"/></div>
            <div class="form-group"><label>Select Button GPIO:</label><input type="number" id="cfgBtnSel" value="25"/></div>
            <div class="form-group"><label>Back Button GPIO:</label><input type="number" id="cfgBtnBack" value="26"/></div>
            <div class="form-group"><label>Battery ADC GPIO (-1 = off):</label><input type="number" id="cfgBatAdc" value="34"/></div>
            <div class="form-group"><label>Power Latch GPIO (-1 = off):</label><input type="number" id="cfgPwrLatch" value="-1"/></div>
          </div>
          <div style="display:flex;gap:10px;margin-top:14px;flex-wrap:wrap">
            <button type="submit" class="btn primary">Save Configuration</button>
            <button type="button" class="btn" onclick="scanI2C()">Scan I2C Bus</button>
          </div>
          <div id="i2cScanResult" style="margin-top:8px;font-size:11px;font-family:monospace;color:#bae6fd"></div>
        </form>
      </div>

      <div id="tab-modes" style="display:none">
        <p style="font-size:12px;color:var(--muted);margin-bottom:12px">Quickly set operational mode or utilities.</p>
        <div style="display:flex;gap:8px;flex-wrap:wrap">
          <button class="btn" onclick="sendNav('SELECT')">Enter Selected Option</button>
          <button class="btn" onclick="sendNav('BACK')">Back / Cancel</button>
        </div>
      </div>

      <div id="tab-system" style="display:none">
        <p style="font-size:12px;color:var(--muted);margin-bottom:12px">System actions and recovery.</p>
        <div style="display:flex;gap:8px;flex-wrap:wrap">
          <button class="btn" onclick="rebootEsp()">Restart ESP32</button>
          <button class="btn danger" onclick="resetDefaults()">Restore Factory Defaults</button>
        </div>
        <div style="margin-top:12px;font-size:11px;color:var(--muted);line-height:1.5">
          <b>Safe Recovery:</b> Hold physical button GPIO 27 for 5s during power-up to restore factory defaults.
        </div>
      </div>
    </div>
  </div>

  <div class="right-col">
    <div class="card">
      <h2><span>LIVE OLED DIGITAL TWIN</span><span style="font-size:10px;color:var(--accent)">128 x 64</span></h2>
      <div class="oled-frame">
        <div class="oled-pins"><span>GND</span><span>VCC</span><span>SCL</span><span>SDA</span></div>
        <div class="screen-housing">
          <canvas id="twinCanvas" width="128" height="64"></canvas>
        </div>
      </div>

      <div class="btn-grid">
        <button class="btn" onclick="sendNav('PREV')">&lang; Prev</button>
        <button class="btn primary" onclick="sendNav('SELECT')">Select &crarr;</button>
        <button class="btn" onclick="sendNav('NEXT')">Next &rang;</button>
        <button class="btn" onclick="sendNav('BACK')">&larr; Back</button>
        <button class="btn" onclick="sendNav('HOME')">&#x2302; Home</button>
        <button class="btn danger" onclick="sendNav('POWER')">&#x23FB; Power</button>
      </div>

      <div class="tactile-wrap">
        <div class="btn-housing" id="btnHousing">
          <button id="singleBtn" class="bigbtn" aria-label="Tactile Pushbutton">
            <span class="btn-cap-icon">&#x25C9;</span>
            <span class="btn-cap-label">PUSH</span>
          </button>
        </div>

        <div class="hold-meter">
          <div class="hold-track">
            <div id="holdBar" class="hold-bar"></div>
          </div>
          <div class="hold-info">
            <span>&#x23F1;&#xFE0F; <b id="heldMs">0 ms</b></span>
            <span id="holdStatus" class="hold-status">READY</span>
          </div>
        </div>

        <div class="quick-chips">
          <button class="chip-btn" onclick="triggerGesture('TAP')">Tap: Next</button>
          <button class="chip-btn" onclick="triggerGesture('DOUBLE')">Double: Back</button>
          <button class="chip-btn" onclick="triggerGesture('HOLD_SELECT')">Hold 1.5s: Select</button>
          <button class="chip-btn" onclick="triggerGesture('HOLD_POWER')">Hold 3s: Power</button>
        </div>
      </div>
    </div>
  </div>
</div>

<script>
const cvs = document.getElementById('twinCanvas');
const ctx = cvs.getContext('2d');
let gState = {botId:1,battery:82,voltage:3.95,batteryReal:true,net:'connected',peers:4,mode:'FOLLOW',page:'HOME',menuIdx:0,cursor:0,holdMs:0,holdTarget:0,uiLocked:false};

function switchTab(name){
  ['status','gpio','modes','system'].forEach(t=>{
    document.getElementById('tab-'+t).style.display=(t===name)?'block':'none';
  });
  document.querySelectorAll('.tabbtn').forEach((b,i)=>{
    b.classList.toggle('active', ['status','gpio','modes','system'][i]===name);
  });
}

function sendNav(action){
  fetch('/api/nav?action='+action, {method:'POST'}).then(()=>fetchState());
}

function fetchState(){
  fetch('/api/state')
    .then(r=>r.json())
    .then(d=>{
      gState = d;
      updateUi();
      renderTwin();
    }).catch(()=>{});
}
setInterval(fetchState, 200);

function updateUi(){
  document.getElementById('botBadge').textContent = 'BOT ' + String(gState.botId).padStart(2,'0');
  document.getElementById('tBotId').textContent = gState.botId;
  document.getElementById('tMode').textContent = gState.mode;
  document.getElementById('tPage').textContent = gState.page;
  document.getElementById('tBat').textContent = gState.battery + '%';
  document.getElementById('tVolt').textContent = (gState.voltage||3.95).toFixed(2) + 'V';
  document.getElementById('batBadge').textContent = gState.battery + '%';
  document.getElementById('modeBadge').textContent = gState.mode;
  document.getElementById('tUiLock').textContent = gState.uiLocked ? 'ACTIVE (MOTORS PAUSED)' : 'IDLE';
  document.getElementById('uiBadge').textContent = gState.uiLocked ? 'AUTONOMOUS PAUSED' : 'ACTIVE';
  document.getElementById('uiBadge').className = 'badge ' + (gState.uiLocked ? 'locked' : 'ok');

  if(gState.batteryReal){
    document.getElementById('tBatTag').textContent = '[LIVE]';
    document.getElementById('tBatTag').className = 'tag-live';
  } else {
    document.getElementById('tBatTag').textContent = '[CALIBRATING]';
    document.getElementById('tBatTag').className = 'tag-mock';
  }
}

function renderTwin(){
  ctx.fillStyle='#000';
  ctx.fillRect(0,0,128,64);
  ctx.fillStyle='#fff';
  ctx.strokeStyle='#fff';
  ctx.lineWidth=1;

  if(gState.page==='BOOT'){
    const hex = [
      [64, 4], [84, 15], [84, 36], [64, 47], [44, 36], [44, 15]
    ];
    const sPath = [
      [74, 15], [63, 11], [53, 17], [53, 23],
      [67, 28], [75, 32], [75, 37], [66, 42], [54, 38]
    ];
    ctx.lineWidth = 2;
    ctx.strokeStyle = '#fff';
    ctx.fillStyle = '#fff';

    ctx.beginPath();
    ctx.moveTo(hex[0][0], hex[0][1]);
    for(let i=1; i<6; i++) ctx.lineTo(hex[i][0], hex[i][1]);
    ctx.closePath();
    ctx.stroke();

    for(let i=0; i<6; i++) {
      ctx.beginPath();
      ctx.arc(hex[i][0], hex[i][1], 2, 0, Math.PI*2);
      ctx.fill();
    }

    ctx.beginPath();
    ctx.moveTo(sPath[0][0], sPath[0][1]);
    for(let i=1; i<9; i++) ctx.lineTo(sPath[i][0], sPath[i][1]);
    ctx.stroke();

    ctx.font = '8px monospace';
    ctx.textAlign = 'center';
    ctx.fillText('SWARM BOT', 64, 62);
    return;
  }

  // Status Bar
  ctx.strokeRect(2,2,11,7);
  ctx.fillRect(4,4,7,3);
  ctx.font='7px monospace';
  ctx.textAlign='left';
  ctx.fillText(gState.peers+'BOT', 15, 8);

  ctx.textAlign='right';
  ctx.fillText(gState.battery+'%', 105, 8);
  ctx.textAlign='left';
  ctx.strokeRect(109,2,15,7);
  ctx.fillRect(124,4,2,3);
  let p = gState.battery;
  if(p>=20) ctx.fillRect(111,3,2,4);
  if(p>=45) ctx.fillRect(114,3,2,4);
  if(p>=70) ctx.fillRect(117,3,2,4);
  if(p>=90) ctx.fillRect(120,3,2,4);
  ctx.beginPath();ctx.moveTo(0,11.5);ctx.lineTo(128,11.5);ctx.stroke();

  if(gState.page==='HOME'){
    ctx.font='bold 14px monospace';
    ctx.textAlign='center';
    ctx.fillText('BOT ' + String(gState.botId).padStart(2,'0'), 64, 24);
    ctx.font='8px monospace';
    ctx.fillText('[' + (gState.mode==='FOLLOW'?'FOLLOW LEADER':'MANUAL MODE') + ']', 64, 35);

    let netTxt = gState.net==='connected' ? (gState.peers+'/'+gState.peers+' CONNECTED') : (gState.net==='connecting' ? 'CONNECTING...' : 'DISCONNECTED');
    ctx.beginPath();
    ctx.arc(38, 43, 2, 0, Math.PI*2);
    ctx.arc(46, 43, 2, 0, Math.PI*2);
    ctx.fill();
    ctx.beginPath();
    ctx.moveTo(38, 43); ctx.lineTo(46, 43);
    ctx.stroke();
    ctx.font='7px monospace';
    ctx.textAlign='left';
    ctx.fillText(netTxt, 52, 46);

    if(gState.holdMs>=200){
      ctx.strokeStyle='#fff';
      ctx.strokeRect(22,54,84,7);
      ctx.fillStyle='#fff';
      let w = Math.min(80, (gState.holdMs/3000)*80);
      ctx.fillRect(24,56,w,3);
    } else {
      ctx.font='6px monospace';
      ctx.textAlign='center';
      ctx.fillText('HOLD:MENU   LONG:PWR', 64, 62);
    }
  } else if(gState.page==='MAIN_MENU'){
    ctx.textAlign='center';
    const items = ['MODES','SETTINGS','CREDITS','POWER','RETURN'];
    for(let i=0; i<5; i++){
      let dotX = 64 - 14 + i*7;
      if(i===(gState.menuIdx||0)){
        ctx.beginPath(); ctx.arc(dotX, 4, 1.5, 0, Math.PI*2); ctx.fill();
      } else {
        ctx.fillRect(dotX, 4, 1, 1);
      }
    }
    ctx.font='9px monospace';
    ctx.fillText('<', 5, 34);
    ctx.fillText('>', 123, 34);
    ctx.strokeRect(52, 14, 24, 24);
    ctx.font='bold 11px monospace';
    ctx.fillText(items[gState.menuIdx||0], 64, 57);
  } else if(gState.page==='MODE_SELECT'){
    ctx.textAlign='center';
    ctx.font='bold 9px monospace';
    ctx.fillText('SELECT MODE', 64, 10);
    ctx.beginPath();ctx.moveTo(0,12.5);ctx.lineTo(128,12.5);ctx.stroke();
    ctx.textAlign='left';
    ctx.font='8px monospace';
    const opts = ['1. Follow Leader','2. Manual Control','< Back to Menu'];
    opts.forEach((o,i)=>{
      let y = 26 + i*12;
      if(i===(gState.cursor||0)){
        ctx.fillRect(6, y-8, 116, 11);
        ctx.fillStyle='#000';
        ctx.fillText(o, 10, y);
        ctx.fillStyle='#fff';
      } else {
        ctx.fillText(o, 10, y);
      }
    });
  } else if(gState.page==='FOLLOW_UTILITIES'){
    ctx.textAlign='center';
    ctx.font='bold 9px monospace';
    ctx.fillText('FOLLOW LEADER', 64, 10);
    ctx.beginPath();ctx.moveTo(0,12.5);ctx.lineTo(128,12.5);ctx.stroke();
    ctx.textAlign='left';
    ctx.font='7px monospace';
    const items = ['Leader: Bot 01','Distance: 20 cm','Drive: RUNNING','< Back to Modes'];
    items.forEach((item,i)=>{
      let y = 24 + i*10;
      if(i===(gState.cursor||0)){
        ctx.fillRect(6, y-7, 116, 9);
        ctx.fillStyle='#000';
        ctx.fillText(item, 10, y);
        ctx.fillStyle='#fff';
      } else {
        ctx.fillText(item, 10, y);
      }
    });
  } else if(gState.page==='MANUAL_UTILITIES'){
    ctx.textAlign='center';
    ctx.font='bold 9px monospace';
    ctx.fillText('MANUAL CONTROL', 64, 10);
    ctx.beginPath();ctx.moveTo(0,12.5);ctx.lineTo(128,12.5);ctx.stroke();
    ctx.textAlign='left';
    ctx.font='7px monospace';
    const items = ['Source: WEB','Speed: 50%','Drive: ACTIVE','< Back to Modes'];
    items.forEach((item,i)=>{
      let y = 24 + i*10;
      if(i===(gState.cursor||0)){
        ctx.fillRect(6, y-7, 116, 9);
        ctx.fillStyle='#000';
        ctx.fillText(item, 10, y);
        ctx.fillStyle='#fff';
      } else {
        ctx.fillText(item, 10, y);
      }
    });
  } else if(gState.page==='SETTINGS'){
    ctx.textAlign='center';
    ctx.font='bold 9px monospace';
    ctx.fillText('SETTINGS', 64, 10);
    ctx.beginPath();ctx.moveTo(0,12.5);ctx.lineTo(128,12.5);ctx.stroke();
    ctx.textAlign='left';
    ctx.font='7px monospace';
    const items = ['Driver: SH1106 0x3C','Input: 1-BTN (P27)','Web: 192.168.4.1','Reset Defaults','< Back to Menu'];
    items.forEach((item,i)=>{
      let y = 23 + i*9;
      if(i===(gState.cursor||0)){
        ctx.fillRect(4, y-7, 120, 8);
        ctx.fillStyle='#000';
        ctx.fillText(item, 8, y);
        ctx.fillStyle='#fff';
      } else {
        ctx.fillText(item, 8, y);
      }
    });
  } else if(gState.page==='CREDITS'){
    ctx.textAlign='center';
    ctx.font='bold 9px monospace';
    ctx.fillText('CREDITS', 64, 10);
    ctx.beginPath();ctx.moveTo(0,12.5);ctx.lineTo(128,12.5);ctx.stroke();
    ctx.font='7px monospace';
    ctx.fillText('SWARM BOT', 64, 25);
    ctx.fillText('TEAM AARYA ATHARVA', 64, 37);
    ctx.fillText('ECS DEPT - VESIT', 64, 49);
  } else if(gState.page==='POWER_CONFIRM'){
    ctx.textAlign='center';
    ctx.font='bold 11px monospace';
    ctx.fillText('POWER OFF?', 64, 42);
    ctx.font='7px monospace';
    ctx.fillText('HOLD 1.5s: CONFIRM', 64, 52);
    ctx.fillText('TAP: CANCEL', 64, 61);
  } else if(gState.page==='SHUTDOWN'){
    ctx.textAlign='center';
    ctx.font='bold 10px monospace';
    ctx.fillText('SAFE TO POWER OFF', 64, 46);
    ctx.font='7px monospace';
    ctx.fillText('SYSTEM HALTED', 64, 58);
  } else {
    ctx.textAlign='center';
    ctx.font='9px monospace';
    ctx.fillText(gState.page, 64, 36);
  }
}

// Single Button Engine with LIVE RAF Timer
const sBtn = document.getElementById('singleBtn');
const btnHousing = document.getElementById('btnHousing');
const heldMsEl = document.getElementById('heldMs');
const holdBarEl = document.getElementById('holdBar');
const holdStatusEl = document.getElementById('holdStatus');

let pStart = 0, isDown = false, tapTimer = null, rafId = null;

function updateHoldLive(){
  if(!isDown) return;
  const elapsed = performance.now() - pStart;
  const ms = Math.round(elapsed);
  heldMsEl.textContent = ms + ' ms';

  const pct = Math.min(100, (elapsed / 3000) * 100);
  holdBarEl.style.width = pct + '%';

  if(elapsed >= 2800){
    btnHousing.className = 'btn-housing active-power';
    holdStatusEl.className = 'hold-status power';
    holdStatusEl.textContent = 'POWER (3s)';
  } else if(elapsed >= 1200){
    btnHousing.className = 'btn-housing active-select';
    holdStatusEl.className = 'hold-status select';
    holdStatusEl.textContent = 'SELECT (1.5s)';
  } else if(elapsed >= 450){
    btnHousing.className = 'btn-housing';
    holdStatusEl.className = 'hold-status';
    holdStatusEl.textContent = 'HOLDING...';
  } else {
    btnHousing.className = 'btn-housing';
    holdStatusEl.className = 'hold-status';
    holdStatusEl.textContent = 'TAP';
  }

  rafId = requestAnimationFrame(updateHoldLive);
}

function downBtn(e){
  e.preventDefault();
  if(isDown) return;
  isDown = true;
  pStart = performance.now();
  sBtn.classList.add('down');
  rafId = requestAnimationFrame(updateHoldLive);
}

function upBtn(e){
  if(!isDown) return;
  isDown = false;
  sBtn.classList.remove('down');
  if(rafId){ cancelAnimationFrame(rafId); rafId = null; }
  btnHousing.className = 'btn-housing';

  const dur = performance.now() - pStart;
  heldMsEl.textContent = Math.round(dur) + ' ms';

  setTimeout(()=>{
    if(!isDown){
      holdBarEl.style.width = '0%';
      holdStatusEl.className = 'hold-status';
      holdStatusEl.textContent = 'READY';
    }
  }, 350);

  if(dur >= 2800){
    triggerGesture('HOLD_POWER');
  } else if(dur >= 1200){
    triggerGesture('HOLD_SELECT');
  } else if(dur >= 30){
    if(tapTimer){
      clearTimeout(tapTimer);
      tapTimer = null;
      triggerGesture('DOUBLE');
    } else {
      tapTimer = setTimeout(()=>{
        tapTimer = null;
        triggerGesture('TAP');
      }, 320);
    }
  }
}

function triggerGesture(g){
  let act = 'NEXT';
  if(g === 'TAP') act = 'NEXT';
  else if(g === 'DOUBLE') act = 'BACK';
  else if(g === 'HOLD_SELECT') act = 'SELECT';
  else if(g === 'HOLD_POWER') act = 'POWER';

  fetch('/api/nav?action=' + act + '&gesture=' + g, {method:'POST'})
    .then(()=>fetchState());
}

sBtn.addEventListener('pointerdown', downBtn);
window.addEventListener('pointerup', upBtn);
window.addEventListener('pointercancel', upBtn);

function rebootEsp(){
  if(confirm('Restart ESP32 now?')) fetch('/api/reboot', {method:'POST'});
}
function resetDefaults(){
  if(confirm('Restore all settings to factory defaults?')) fetch('/api/reset', {method:'POST'}).then(()=>alert('Defaults restored.'));
}
function scanI2C(){
  document.getElementById('i2cScanResult').textContent = 'Scanning...';
  fetch('/api/i2c-scan')
    .then(r=>r.json())
    .then(d=>{
      document.getElementById('i2cScanResult').textContent = 'Found I2C addresses: ' + (d.addresses?d.addresses.join(', '):'None');
    }).catch(()=>{ document.getElementById('i2cScanResult').textContent = 'Scan failed.'; });
}
function saveConfig(e){
  e.preventDefault();
  const params = new URLSearchParams();
  params.append('botId', document.getElementById('cfgBotId').value);
  params.append('oledDriver', document.getElementById('cfgDriver').value);
  params.append('oledI2cAddr', document.getElementById('cfgAddr').value);
  params.append('pinOledSda', document.getElementById('cfgSda').value);
  params.append('pinOledScl', document.getElementById('cfgScl').value);
  params.append('inputMode', document.getElementById('cfgInMode').value);
  params.append('pinBtnSingle', document.getElementById('cfgBtnSingle').value);
  params.append('pinBtnPrev', document.getElementById('cfgBtnPrev').value);
  params.append('pinBtnNext', document.getElementById('cfgBtnNext').value);
  params.append('pinBtnSelect', document.getElementById('cfgBtnSel').value);
  params.append('pinBtnBack', document.getElementById('cfgBtnBack').value);
  params.append('pinBatteryAdc', document.getElementById('cfgBatAdc').value);
  params.append('pinPowerLatch', document.getElementById('cfgPwrLatch').value);

  const a = document.getElementById('cfgAlert');
  fetch('/api/config', {method:'POST', body:params})
    .then(r=>r.json())
    .then(d=>{
      if(d.success){
        a.className = 'alert success';
        a.textContent = d.message || 'Saved! Reboot to apply.';
      } else {
        a.className = 'alert error';
        a.textContent = d.error || 'Failed to save.';
      }
    }).catch(err=>{
      a.className = 'alert error';
      a.textContent = 'Request failed.';
    });
}
</script>
</body>
</html>)rawliteral";

