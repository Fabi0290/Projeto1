import serial
import syslog
import datetime
from twilio.rest import Client

# Conf serial
PORT = "/dev/ttyACM0"  
BAUD_RATE = 115200

# Conf Twilio
TWILIO_SID = "MYSID"
TWILIO_AUTH_TOKEN = "MYTOKEN"
FROM_PHONE = "MYTWILIOPHONE"
TO_PHONE = "MYPHONENUMBER"

# Criar cliente Twilio
client = Client(TWILIO_SID, TWILIO_AUTH_TOKEN)

# FunÃ§Ã£o de logs
def criar_logs(estado):
    timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    log_message = f"projeto1 - Status: {estado}"
    syslog.syslog(syslog.LOG_INFO, log_message)
    print(log_message)

# Conectar Ã  porta serial
with serial.Serial(PORT, BAUD_RATE, timeout=1) as ser:
    print(f"Conectado ao dispositivo na porta {PORT}")
    estado_atual = ""
    
    # Loop para verificar dados da porta serial
    while True:
        # Leitura de linha de dados
        linha = ser.readline().decode('utf-8').strip()
        
        if linha:
            print(f"Recebido: {repr(linha)}")
            
            # Verifica se a mensagem indica "desligado"
            if "ligado: false" in linha.lower():
                print("Status detectado: desligado")
                if estado_atual != "desligado":
                    estado_atual = "desligado"
                    criar_logs("desligado")
                    
                    # Tenta enviar mensagem
                    try:
                        message = client.messages.create(
                            to=TO_PHONE,
                            from_=FROM_PHONE,
                            body="Alerta: dispositivo desligado"
                        )
                        print("Mensagem enviada com sucesso!")
                    except Exception as e:
                        print(f"Erro ao enviar mensagem: {e}")
            
            # Verifica se a mensagem indica "ligado"
            elif "ligado: true" in linha.lower():
                print("Status detectado: ligado")
                if estado_atual != "ligado":
                    estado_atual = "ligado"
                    criar_logs("ligado")
