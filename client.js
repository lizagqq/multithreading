const net = require('net');
const readline = require('readline');  //readline — это модуль для чтения ввода с командной строки

const rl = readline.createInterface({  //readline.createInterface создает интерфейс для чтения и записи в командную строку
  input: process.stdin,
  output: process.stdout,
  prompt: 'Введите сообщение: '  // функция prompt используется для получения данных задает текст который будет выводиться перед ожиданием ввода
});

const client = net.createConnection({ port: 8080 }, () => {
  console.log('Connected to server');
  console.log(`Local IP: ${client.localAddress}, Local Port: ${client.localPort}`);
  console.log(`Server IP: ${client.remoteAddress}, Server Port: ${client.remotePort}`);
  rl.prompt();
});

rl.on('line', (line) => {
  if (line.trim().toLowerCase() === 'exit') {
    client.end(); //  разрывание соединения
    rl.close();
  } else {
    client.write(line);
  }
});

client.on('data', (data) => {
  console.log(`Ответ с сервера: ${data.toString().trim()}`);
  rl.prompt();
});

client.on('end', () => {
  console.log('Disconnected from server');
  rl.close();
});

client.on('error', (err) => {
  console.error(`Client error: ${err.message}`);
  rl.close();
});

client.on('close', (hadError) => {
  console.log(`Connection closed${hadError ? ' due to an error' : ''}`);
  rl.close();
});
