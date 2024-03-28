import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

def enviar_email(remetente, destinatario, assunto, corpo, senha):
    # Configurações do servidor SMTP
    servidor_smtp = 'smtp.example.com'
    porta_smtp = 587  # Porta para conexão TLS
    
    # Criando a mensagem
    mensagem = MIMEMultipart()
    mensagem['From'] = remetente
    mensagem['To'] = destinatario
    mensagem['Subject'] = assunto
    
    # Adicionando o corpo do e-mail
    mensagem.attach(MIMEText(corpo, 'plain'))
    
    # Iniciando conexão SMTP
    servidor = smtplib.SMTP(servidor_smtp, porta_smtp)
    servidor.starttls()
    
    # Login no servidor SMTP
    servidor.login(remetente, senha)
    
    # Enviando e-mail
    servidor.sendmail(remetente, destinatario, mensagem.as_string())
    
    # Finalizando conexão SMTP
    servidor.quit()

# Preencha com suas informações
remetente = 'seu_email@example.com'
destinatario = 'destinatario@example.com'
assunto = 'Assunto do e-mail'
corpo = 'Corpo do e-mail'
senha = 'sua_senha'

enviar_email(remetente, destinatario, assunto, corpo, senha)
