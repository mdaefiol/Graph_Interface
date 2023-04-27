import serial
import serial.tools.list_ports
import PySimpleGUI as sg


# Define a classe a ser utilizada
class SerialApp():
    def __init__(self):
        self.serial_port = serial.Serial()
        self.baudrates = [9600, 115200]
        self.portlist = []

    # Update das portas COM disponíveis
    def update_port_list(self):
        self.portlist = [port.device for port in serial.tools.list_ports.comports()]

    # Conecta a serial
    def connect_serial(self, port, baudrate):
        self.serial_port.port = port
        self.serial_port.baudrate = baudrate
        self.serial_port.open()

    # Leitura de dados
    def read_serial(self):
        data_read = self.serial_port.read(8)
        return data_read

    # Finaliza a porta serial
    def close_serial(self):
        self.serial_port.close()


# Salva o arquivo .txt dentro da pasta do projeto
def save_to_file(data, filename):
    with open(filename, 'a') as file:
        hex_data = data
        hex_data_with_comma = ',\n'.join(hex_data[i:i + 2] for i in range(0, len(hex_data), 2))
        file.write(hex_data_with_comma)
    show_confirmation_window()

# Exibe uma janela de confirmação
def show_confirmation_window():
    layout = [
        [sg.Text('Dados salvos com sucesso!')],
        [sg.Button('OK', key='-OK-')]
    ]
    window = sg.Window('Confirmação', layout)
    while True:
        event, values = window.read()
        if event == '-OK-' or event == sg.WIN_CLOSED:
            break
    window.close()

def show_message(window, message):
    sg.popup_no_wait(message, title='Fim da Leitura')

def main():
    serial_app = SerialApp()
    serial_app.update_port_list()

    layout = [
        [sg.Text("Porta Serial:"), sg.Combo(serial_app.portlist, size=(20, 1), key="-PORT-")],
        [sg.Text("Baudrate:"), sg.Combo(serial_app.baudrates, default_value=9600, size=(20, 1), key="-BAUDRATE-")],
        [sg.Button("Conectar", key="-CONNECT-"), sg.Button("Desconectar", key="-DISCONNECT-", disabled=True)],
        [sg.Text("Dados Recebidos:"), sg.Multiline(size=(50, 10), key="-DATA-")],
        [sg.Button("Salvar Dados", key="-SAVE-", disabled=True)],
        [sg.Text(size=(40, 1), key="-FINALIZED-")]
    ]

    window = sg.Window("Receber Dados via Serial", layout)

    connected = False
    while True:
        event, values = window.read(timeout=1000)
        if event == sg.WIN_CLOSED:
            break

        if event == "-CONNECT-":
            port = values["-PORT-"]
            baudrate = int(values["-BAUDRATE-"])
            try:
                serial_app.connect_serial(port, baudrate)
                connected = True
                window["-CONNECT-"].update(disabled=True)
                window["-DISCONNECT-"].update(disabled=False)
                window["-SAVE-"].update(disabled=True)         # Desabilita o botão "Salvar Dados"
            except Exception as e:
                sg.popup_error(f"Erro ao conectar na porta {port}: {e}")

        if event == "-DISCONNECT-":
            serial_app.close_serial()
            connected = False
            window["-CONNECT-"].update(disabled=False)
            window["-DISCONNECT-"].update(disabled=True)
            window["-SAVE-"].update(disabled=False)               # Habilita o botão "Salvar Dados"

        if event == "-SAVE-":
            data = window["-DATA-"].get()                         # Obtém os dados recebidos
            save_to_file(data, "data.txt")

        if connected:
            try:
                data = serial_app.read_serial().hex()
                if data:
                    window["-DATA-"].print(data)

            except Exception as e:
                sg.popup_error(f"Erro ao ler dados da porta {port}: {e}")

        if event == sg.WIN_CLOSED:
            break

    window.close()


if __name__ == "__main__":
    main()