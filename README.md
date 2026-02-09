# README

codespace environment: https://codespaces.new/Danilo0liveira/str_proj_01

# 🤖 Simulação de Célula de Manufatura Automatizada

Este projeto implementa o controle de uma célula de manufatura utilizando *** em C. A simulação coordena duas máquinas de processamento, um robô industrial compartilhado e um buffer de saída limitado, resolvendo problemas clássicos de sincronização como **Deadlock** e **Condição de Corrida**.

---

## 🏗️ Estrutura do Sistema

O sistema é composto por três agentes principais que operam simultaneamente:

* **Máquinas (M1 e M2):** Estações independentes que processam matéria-prima.
* **Robô Industrial:** Único agente de transporte que move peças das máquinas para o buffer.
* **Esteira (Buffer de Saída):** Zona de armazenamento com capacidade máxima para **2 peças**.

---

## 🚦 Lógica de Sincronização (Semáforos)

Para garantir que as restrições físicas da planta sejam respeitadas, foram utilizados semáforos da biblioteca `semaphore.h`:

| Semáforo | Função | Descrição |
| :--- | :--- | :--- |
| `s` | **Contador de Itens** | Indica ao robô quantas peças estão prontas nas máquinas. |
| `m[id]` | **Espaço na Máquina** | Garante que a máquina só produza se o seu slot de saída estiver vazio. |
| `full` | **Controle de Buffer** | Gerencia os 2 slots da esteira, impedindo o transbordamento (Overflow). |
| `mutex` | **Exclusão Mútua** | Impede que o robô e a esteira alterem o contador do buffer final ao mesmo tempo. |



---

## 🛠️ Detalhes de Implementação

### 1. Evitando o Transbordamento (Overflow)
O robô só deposita uma peça se o semáforo `full` (inicializado em 2) permitir. Caso o buffer esteja cheio, o robô aguarda até que a esteira remova uma peça.

### 2. Evitando o Bloqueio (Deadlock)
O sistema garante fluidez pois a thread da esteira (`conveyor`) atua como um consumidor constante, garantindo que o recurso "espaço no buffer" seja eventualmente liberado.

### 3. Exclusão Mútua
As variáveis globais, como `buffer_out`, são protegidas por um **Mutex**, garantindo que a integridade dos dados seja mantida mesmo com múltiplas threads acessando os recursos.

---

