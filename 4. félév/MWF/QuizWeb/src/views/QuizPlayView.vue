<script setup lang="ts">
import { ref, onMounted, onUnmounted } from 'vue'
import { quizService } from '../services/quizService'
import type { QuizResponseForPlayerDto } from '../Models/responseDtos'
import { usePlayerStore } from '../stores/playerStore'
import { useRouter } from 'vue-router'
import { quizHubService } from '../services/quizHubService'

const playerStore = usePlayerStore();
const router = useRouter();

const quiz = ref<QuizResponseForPlayerDto | null>(null)
const isLoading = ref(false)
const error = ref('')
const newMessage = ref("");
let correctAnswerIndex = ref(-1);

const loadQuiz = async () => {
    isLoading.value = true
    error.value = ''

    try {
        const data = await quizService.getQuizByJoinCode({
            playerName: playerStore.playerName,
            joinCode: playerStore.joinCode
        })

        quiz.value = data
        if (quiz.value.currentQuestionIndex == 0) { playerStore.setQuestionIndex(quiz.value.currentQuestionIndex); }
        if (playerStore.getQuestionIndex() != quiz.value.currentQuestionIndex) {
            playerStore.clearSelectedAnswerIndex();
            playerStore.setQuestionIndex(quiz.value.currentQuestionIndex);
        }
    } catch (e: any) {
        playerStore.clearPlayerStore();
        console.log(e)
        router.push("/");
    } finally {
        isLoading.value = false
    }
}

const currentQuestion = () => {
    return quiz.value?.questions[quiz.value.currentQuestionIndex];
}

const handleAnwserSelect = (index: number) => {
    if (playerStore.getSelectedAnswerIndex() != null) return;
    playerStore.setSelectedAnswerIndex(index);
}

const sendMessage = async () => {
    if (!newMessage.value.trim()) return;

    await quizService.sendMessage({
        playerName: playerStore.playerName,
        joinCode: playerStore.joinCode,
        message: newMessage.value
    });

    newMessage.value = "";
};

onMounted(async () => {
    quizHubService.connect(playerStore.joinCode);

    quizHubService.onQuestionChanged(async () => {
        await loadQuiz();
    });

    quizHubService.onQuestionClosed(async (_correctAnswerIndex: number) => {
        await loadQuiz();
        correctAnswerIndex.value = _correctAnswerIndex;
    });

    quizHubService.onQuizEnded(() => {
        router.replace('/');
    });

    quizHubService.onMessageSent((message: string) => {
        if (quiz.value == null) return;
        quiz.value.messages.push(message);
    });

    await loadQuiz();
})

onUnmounted(async () => {
    await quizHubService.disconnect(playerStore.joinCode);
    playerStore.clearPlayerStore();
});

</script>



<template>
  <div class="container py-4 py-lg-5">
    <div class="row g-4">
      <div class="col-12 col-md-8">
        <div class="card border-0 shadow-sm rounded-4 h-100">
          <div class="card-body p-4 p-lg-5">
            <div v-if="isLoading" class="text-center text-muted py-5">
              Loading question...
            </div>

            <div v-else-if="error" class="alert alert-danger rounded-3">
              {{ error }}
            </div>

            <div v-else-if="quiz?.status == 'Lobby'" class="text-center py-5">
              <h3 class="fw-bold mb-2">Lobby</h3>
              <p class="text-muted mb-0">Waiting for the quiz to start.</p>
            </div>

            <div v-else-if="currentQuestion() && quiz?.status == 'InProgress'">
              <div class="mb-4">
                <span class="badge bg-primary rounded-pill mb-3">Question</span>
                <h3 class="fw-bold mb-0">{{ currentQuestion()?.title }}</h3>
              </div>

              <div class="list-group mb-4 rounded-4 overflow-hidden">
                <button
                  v-for="(answer, index) in currentQuestion()?.answers"
                  :key="index"
                  class="list-group-item list-group-item-action py-3 px-4"
                  :class="{ 'active': playerStore.selectedAnswerIndex == index, 'disabled': playerStore.selectedAnswerIndex !== null && playerStore.selectedAnswerIndex !== index }"
                  @click="playerStore.selectedAnswerIndex === null && (handleAnwserSelect(index))"
                  :disabled="playerStore.selectedAnswerIndex !== null || !currentQuestion()?.isOpen"
                >
                  {{ answer }}
                </button>
              </div>

              <div
                v-if="currentQuestion() && !currentQuestion()?.isOpen && quiz?.status == 'InProgress'"
                class="alert alert-success rounded-3 mb-0"
              >
                <strong>Correct answer:</strong>
                {{ currentQuestion()?.answers[correctAnswerIndex] }}
              </div>
            </div>

            <div v-else-if="quiz?.status == 'Finished'" class="text-center py-5">
              <h3 class="fw-bold mb-2">Finished</h3>
              <p class="text-muted mb-0">The quiz has ended.</p>
            </div>
          </div>
        </div>
      </div>

      <div class="col-12 col-md-4">
        <div class="card h-100 border-0 shadow-sm rounded-4 d-flex flex-column overflow-hidden">
          <div class="card-header bg-white border-0 p-4 pb-2">
            <strong class="fs-5">Chat</strong>
          </div>

          <div class="card-body overflow-auto px-4" style="max-height: 400px;">
            <div v-if="!quiz?.messages || quiz.messages.length === 0" class="text-center text-muted py-4">
              <small>No messages yet</small>
            </div>

            <div v-for="(msg, index) in quiz?.messages" :key="index" class="mb-2">
              <div class="p-3 bg-light rounded-3">
                {{ msg }}
              </div>
            </div>
          </div>

          <div class="card-footer bg-white border-0 p-4">
            <div class="input-group">
              <input
                v-model="newMessage"
                type="text"
                class="form-control"
                placeholder="Type a message..."
                @keyup.enter="sendMessage"
              />
              <button class="btn btn-primary px-3" @click="sendMessage">
                Send
              </button>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>