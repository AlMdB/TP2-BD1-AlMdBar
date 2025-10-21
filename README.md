# TP2- Bancos de Dados 1: Sistema de Armazenamento e Indexação



# Estrutura dos Diretórios

```
/TP2-BD1-AlMdBar
├── docker-compose.yml
├── Dockerfile
├── include
│   ├── Block.hpp
│   ├── BlockStructInfo.hpp
│   ├── BTreeP.hpp
│   ├── Chronometer.hpp
│   ├── FileReader.hpp
│   ├── Logger.hpp
│   ├── Register.hpp
│   └── utils
│       ├── DataDict.hpp
│       ├── DiskInfo.hpp
│       ├── ParseSchema.hpp
│       └── RegisterType.hpp
├── init.sh
├── makefile
├── README.md
├── src
│   └── utils
│       ├── Chronometer.cpp
│       ├── FileReader.cpp
│       ├── Logger.cpp
│       └── test1.cpp
└── test.sh

```

# Comandos Make

# Compilar todos os programas

# Compilar testes

# Comandos Locais

```
./bin/upload /path/to/data.csv

./bin/findrec 123

./bin/seek1 456

./bin/seek2 "Titulo do Artigo"
```

# Limpar Arquivos Compilados
```
make clean
```

# Build e Execução com Docker
```
make docker-build
```


# Configurações Docker

# Variáveis de Ambiente(Docker)
```
CSV_PATH=/data/input.csv
DATA_DIR=/data/db
LOG_LEVEL=info
```