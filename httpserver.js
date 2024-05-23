const http = require('http');

// Проверка наличия параметра URL
if (process.argv.length !== 3) {  
  console.error('Usage: node httpserver.js <URL>');
  process.exit(1);
}

const url = process.argv[2];
let responseData = '';
let lines = [];  //массив строк
let lineCount = 0;

const showNextLines = () => {
  const remainingLines = lines.slice(lineCount, lineCount + 25);
  process.stdout.write(remainingLines.join('\n') + '\n');
  lineCount += 25;

  if (lineCount < lines.length) {
    console.log('\nPress space to scroll down');
    process.stdin.setRawMode(true);
    process.stdin.resume();
    process.stdin.once('data', (key) => {
      if (key == ' ') {
        process.stdout.write('\033[2J\033[H'); // Очистка терминала
        process.stdin.setRawMode(false);
        process.stdin.pause();
        showNextLines();
      } else {
        process.stdin.setRawMode(false);
        process.stdin.pause();
      }
    });
  } else {
    console.log('\n\n--- конец! ---');
  }
};

// Выполнение HTTP-запроса
const req = http.get(url, (res) => {
  res.on('data', (chunk) => {
    responseData += chunk;
    lines = responseData.split('\n');
  });

  res.on('end', () => {
    if (lineCount === 0) {
      showNextLines();
    }
  });
});

// Обработка ошибок при выполнении запроса
req.on('error', (err) => {
  console.error(`Error: ${err.message}`);
});
