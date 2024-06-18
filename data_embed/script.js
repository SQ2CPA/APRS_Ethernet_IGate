// Custom scripts

let currentSettings = null;

function backupSettings() {
    const data =
        "data:text/json;charset=utf-8," +
        encodeURIComponent(JSON.stringify(currentSettings));
    const a = document.createElement("a");
    a.setAttribute("href", data);
    a.setAttribute("download", "configuration.json");
    a.click();
}

document.getElementById("backup").onclick = backupSettings;

document.getElementById("restore").onclick = function (e) {
    e.preventDefault();

    document.querySelector("input[type=file]").click();
};

document.querySelector("input[type=file]").onchange = function () {
    const files = document.querySelector("input[type=file]").files;

    if (!files.length) return;

    const file = files.item(0);

    const reader = new FileReader();
    reader.readAsText(file);
    reader.onload = () => {
        const data = JSON.parse(reader.result);

        loadSettings(data);
    };
};

function fetchSettings() {
    fetch("/configuration.json")
        .then((response) => response.json())
        .then((settings) => {
            loadSettings(settings);
        })
        .catch((err) => {
            console.error(err);

            alert(`Failed to load configuration`);
        });
}

function loadSettings(settings) {
    currentSettings = settings;

    document.getElementById("callsign").value = settings.callsign;

    document.getElementById("aprs_is.active").checked = settings.aprs_is.active;
    document.getElementById("aprs_is.server").value = settings.aprs_is.server;
    document.getElementById("aprs_is.port").value = settings.aprs_is.port;
    document.getElementById("aprs_is.passcode").value = settings.aprs_is.passcode;

    document.getElementById("tnc.enableServer").checked = settings.tnc.enableServer;

    document.getElementById("ports.A.baudrate").value = settings.ports.A.baudrate;
    document.getElementById("ports.A.callsign").value = settings.ports.A.callsign;

    document.getElementById("ports.B.baudrate").value = settings.ports.B.baudrate;
    document.getElementById("ports.B.callsign").value = settings.ports.B.callsign;
}

function showToast(message) {
    const el = document.querySelector('#toast');

    el.querySelector('.toast-body').innerHTML = message;

    (new bootstrap.Toast(el)).show();
}

document.getElementById('reboot').addEventListener('click', function (e) {
    e.preventDefault();

    fetch("/action?type=reboot", { method: "POST" });

    showToast("Your device will be rebooted in a while");
});

const form = document.querySelector("form");

const saveModal = new bootstrap.Modal(document.getElementById("saveModal"), {
    backdrop: "static",
    keyboard: false,
});

const savedModal = new bootstrap.Modal(
    document.getElementById("savedModal"),
    {}
);

function checkConnection() {
    const controller = new AbortController();

    setTimeout(() => controller.abort(), 2000);

    fetch("/status?_t=" + Date.now(), { signal: controller.signal })
        .then(() => {
            saveModal.hide();

            savedModal.show();

            setTimeout(function () {
                savedModal.hide();
            }, 3000);

            fetchSettings();
        })
        .catch((err) => {
            setTimeout(checkConnection, 0);
        });
}

form.addEventListener("submit", async (event) => {
    event.preventDefault();

    fetch(form.action, {
        method: form.method,
        body: new FormData(form),
    });

    saveModal.show();

    setTimeout(checkConnection, 2000);
});

fetchSettings();

function loadReceivedPackets(packets) {
    if (packets) {
        document.querySelector('#received-packets tbody').innerHTML = '';

        const container = document.querySelector("#received-packets tbody");

        container.innerHTML = '';

        const date = new Date();

        packets.forEach((packet) => {
            const element = document.createElement("tr");

            date.setTime(packet.millis);

            const p = date.toUTCString().split(' ')
        
            element.innerHTML = `
                        <td>${p[p.length-2]}</td>
                        <td>${packet.packet}</td>
                        <td>${packet.port}</td>
                    `;

            container.appendChild(element);
        })
    }

    setTimeout(fetchReceivedPackets, 15000);
}

function fetchReceivedPackets() {
    fetch("/received-packets.json")
    .then((response) => response.json())
    .then((packets) => {
        loadReceivedPackets(packets);
    })
    .catch((err) => {
        console.error(err);

        console.error(`Failed to load received packets`);
    });
}

document.querySelector('a[href="/received-packets"]').addEventListener('click', function (e) {
    e.preventDefault();

    document.getElementById('received-packets').classList.remove('d-none');
    document.getElementById('configuration').classList.add('d-none');
    
    document.querySelector('button[type=submit]').remove();

    fetchReceivedPackets();
})