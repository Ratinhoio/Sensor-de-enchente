
from fastapi import FastAPI
from fastapi.responses import FileResponse
import serial
import threading
import time

app = FastAPI()

dados = {
    "chovendo": False,
    "analogico": 0,
    "distancia": -1,
    "alerta": False,
    "conectado": False
}

lock = threading.Lock()


def ler_arduino():
    while True:
        try:
            print("Conectando ao Arduino...")

            with serial.Serial('/dev/ttyACM0', 9600, timeout=2) as arduino:
                time.sleep(2)
                arduino.reset_input_buffer()

                print("Arduino conectado!")

                while True:
                    leitura = arduino.readline().decode(
                        'ascii', errors='ignore'
                    ).strip()

                    if not leitura:
                        continue

                    try:
                        chuva, analogico, distancia, alerta = leitura.split(',')

                        chuva = int(chuva)
                        analogico = int(analogico)
                        distancia = float(distancia)
                        alerta = int(alerta)

                        with lock:
                            dados["chovendo"] = chuva == 1
                            dados["analogico"] = analogico
                            dados["distancia"] = distancia
                            dados["alerta"] = alerta == 1
                            dados["conectado"] = True

                        print(
                            f"Chuva: {chuva} | "
                            f"Analógico: {analogico} | "
                            f"Distância: {distancia:.2f} cm | "
                            f"Alerta: {alerta}"
                        )

                    except (ValueError, TypeError):
                        print("Leitura inválida:", repr(leitura))

        except Exception as erro:
            with lock:
                dados["conectado"] = False

            print("Erro na comunicação:", erro)
            time.sleep(3)


@app.on_event("startup")
def iniciar_leitura():
    thread = threading.Thread(
        target=ler_arduino,
        daemon=True
    )
    thread.start()


@app.get("/")
def pagina_inicial():
    return FileResponse("index.html")


@app.get("/api/status")
def status():
    with lock:
        return dados.copy()
