import serial
import serial.tools.list_ports
import PySimpleGUI as sg
import struct

# Define a classe a ser utilizada
class SerialApp():
    def __init__(self):
        self.serial_port = serial.Serial()
        self.baudrates = [9600, 115200]
        self.portlist = []
        self.connected_port = None

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
        data_read = self.serial_port.read(4)
        return data_read

    # Finaliza a porta serial
    def close_serial(self):
        self.serial_port.close()


def store_data_in_columns(data):
    num_columns = 4
    columns = [[] for _ in range(num_columns)]

    for i, item in enumerate(data):
        column_index = i % num_columns
        columns[column_index].append(item)

    return columns

def save_data_to_file(columns, filename):
    with open(filename, 'w') as output_file:
        max_length = max(len(column) for column in columns)

        for i in range(max_length):
            line = '\t'.join(column[i] if i < len(column) else '' for column in columns)
            output_file.write(line + '\n')
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
        [sg.Button("Salvar Dados", key="-SAVE-", disabled=True), sg.Button("START", key="-START-", disabled=True)],
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
                serial_app.connected_port = port               # Armazena a porta conectada na variável "connected_port"
                window["-CONNECT-"].update(disabled=True)      # Desabilita o botão conectar
                window["-DISCONNECT-"].update(disabled=False)  # Habilita botão desconectar
                window["-SAVE-"].update(disabled=True)         # Desabilita o botão "Salvar Dados"
                window["-START-"].update(disabled=False)       # Habilita o botão de enviar o comando pela serial
                # Salva os dados após desconectar da porta serial

            except Exception as e:
                sg.popup_error("Erro ao conectar na porta")

        if event == "-DISCONNECT-":
            serial_app.close_serial()
            connected = False
            window["-CONNECT-"].update(disabled=False)            # Habilita o botão conectar
            window["-DISCONNECT-"].update(disabled=True)          # Desabilita o botão desconectar
            window["-SAVE-"].update(disabled=False)               # Habilita o botão "Salvar Dados"
            window["-START-"].update(disabled=True)               # Desabilita o botão "START"

        if event == "-SAVE-":
            data = values["-DATA-"].split("\n")                    # Obtém os dados recebidos como uma lista
            result = store_data_in_columns(data)                  # Organiza os dados em colunas
            save_data_to_file(result, "output.txt")                # Salva os dados em um arquivo de texto

        if event == "-START-":
            if serial_app.connected_port:
                serial_app.serial_port.write(bytes([0x01]))
                connected = True
                # Envie um sinal pela serial usando a porta armazenada em "connected_port"
            else:
                sg.popup_error("Nenhuma porta serial conectada.")

        if connected:
            try:
                data = serial_app.read_serial().hex()
                bytes_data = bytes.fromhex(data)
                float_value = struct.unpack('!f', bytes_data)[0]
                if data:
                    window["-DATA-"].print(float_value)
            except Exception as e:
                # Trata qualquer exceção que possa ocorrer durante a leitura e conversão dos dados
                sg.popup_error("Erro ao ler dados da porta.")

        if event == sg.WIN_CLOSED:
            break

    window.close()


if __name__ == "__main__":
    main()