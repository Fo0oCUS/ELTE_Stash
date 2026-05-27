<script setup lang="ts">
import { ref, onMounted } from 'vue';
import { quizService } from '../services/quizService';
import { useRoute } from 'vue-router';
import type { QuizResponseDto } from '../Models/responseDtos';
import { QuizStatus } from '../Models/enums';

const route = useRoute();
const errorMessage = ref('');
const quiz = ref<QuizResponseDto | null>(null);
const isLoading = ref(false);
const isPublishing = ref(false);
const quizId = ref(Number(route.params.id));

const getStatusClass = (status: string) => {
  switch (status) {
    case QuizStatus.waitingToBePublished:
      return 'bg-secondary';
    case QuizStatus.inProgress:
      return 'bg-warning text-dark';
    case QuizStatus.finished:
      return 'bg-success';
    default:
      return 'bg-light text-dark';
  }
};

const loadQuiz = async () => {
  errorMessage.value = '';
  isLoading.value = true;

  try {
    await getQuiz();
  } catch (error: any) {
    errorMessage.value = error.message;
    console.error(error);
  } finally {
    isLoading.value = false;
  }
};

onMounted(async () => {
  await loadQuiz();
});

const handlePublish = async () => {
  errorMessage.value = '';
  isPublishing.value = true;

  try {
    const response = await quizService.publishQuiz(quizId.value);
    quiz.value = response;
  } catch (error: any) {
    errorMessage.value = error.message;
    console.error(error);
  } finally {
    isPublishing.value = false;
  }
};

const copyJoinCode = async () => {
  if (!quiz.value?.joinCode) return

  try {
    await navigator.clipboard.writeText(quiz.value.joinCode)
  } catch (err) {
    console.error('Copy failed', err)
  }
}

const handleNext = async () => {
  try {
    await quizService.nextQuestion(quizId.value)
    await getQuiz();
  } catch (e: any) {
    errorMessage.value = e.message
  }
}

const getQuiz = async () => {
  const response = await quizService.getQuiz(quizId.value);
  quiz.value = response;
}

const handleEnd = async () => {
  try {
    await quizService.endQuiz(quizId.value)
    await getQuiz();
  } catch (e: any) {
    errorMessage.value = e.message
  }
}

const handleCloseQuestion = async () => {
  try {
    await quizService.closeCurrentQuestion(quizId.value);
    await getQuiz();
  } catch (e: any) {
    errorMessage.value = e.message;
  }
}

const getCurrentQuestion = () => {
  return quiz.value?.questions[quiz.value.currentQuestionIndex];
}

const isLastQuestion = () => {
  return quiz.value?.currentQuestionIndex == (quiz.value?.questions.length ?? -1) - 1;
}

</script>

<template>
  <div class="container py-4 py-lg-5">
    <div v-if="isLoading" class="card border-0 shadow-sm rounded-4">
      <div class="card-body p-4 text-center text-muted">
        Loading quiz...
      </div>
    </div>

    <div v-else-if="errorMessage" class="alert alert-danger rounded-3 shadow-sm">
      {{ errorMessage }}
    </div>

    <div v-else-if="quiz">
      <div class="card border-0 shadow-sm rounded-4 mb-4">
        <div class="card-body p-4">
          <div class="d-flex flex-column flex-lg-row justify-content-between align-items-lg-start gap-4">
            <div>
              <h1 class="fw-bold mb-2">{{ quiz.title }}</h1>
              <p class="text-muted mb-0">
                Current question index: {{ quiz.currentQuestionIndex + 1 }}
              </p>
            </div>

            <div class="text-lg-end">
              <div class="mb-3">
                <span class="badge rounded-pill me-2" :class="getStatusClass(quiz.status)">
                  {{ quiz.status }}
                </span>
                <span class="badge rounded-pill" :class="quiz.isPublished ? 'bg-primary' : 'bg-dark'">
                  {{ quiz.isPublished ? 'Published' : 'Draft' }}
                </span>
              </div>

              <button v-if="!quiz.isPublished" class="btn btn-primary btn-sm rounded-pill px-4" :disabled="isPublishing"
                @click="handlePublish">
                {{ isPublishing ? 'Publishing...' : 'Publish quiz' }}
              </button>
            </div>
          </div>
        </div>
      </div>

      <div v-if="quiz.isPublished" class="alert alert-success rounded-4 shadow-sm mt-3 p-4">
        <div class="d-flex flex-column flex-lg-row justify-content-between align-items-lg-center gap-4">
          <div>
            <div class="fw-bold fs-5 mb-2">Quiz is live</div>

            <div v-if="quiz.joinCode" class="d-flex flex-column flex-sm-row align-items-sm-center gap-2">
              <input class="form-control w-auto" :value="quiz.joinCode" readonly />

              <button v-if="quiz.joinCode" class="btn btn-outline-dark btn-sm rounded-pill px-3" @click="copyJoinCode">
                Copy
              </button>
            </div>

            <div v-else class="text-danger">
              No join code available
            </div>
          </div>

          <div class="d-flex flex-wrap gap-2 justify-content-lg-end">
            <button v-if="getCurrentQuestion() == undefined && quiz.currentQuestionIndex == -1"
              class="btn btn-success btn-sm rounded-pill px-4" @click="handleNext">
              Start Quiz
            </button>

            <button
              v-else-if="quiz.status == QuizStatus.inProgress && !getCurrentQuestion()?.isOpen && !isLastQuestion()"
              class="btn btn-success btn-sm rounded-pill px-4" @click="handleNext">
              Next Question
            </button>

            <button v-else-if="(quiz.status == QuizStatus.inProgress && getCurrentQuestion()?.isOpen)"
              class="btn btn-warning btn-sm rounded-pill px-4" @click="handleCloseQuestion">
              Close Question
            </button>

            <button
              v-else-if="quiz.status == QuizStatus.inProgress && !getCurrentQuestion()?.isOpen && isLastQuestion()"
              class="btn btn-danger btn-sm rounded-pill px-4" @click="handleEnd">
              Finish Quiz
            </button>
          </div>
        </div>
      </div>

      <div v-if="quiz.questions.length === 0" class="alert alert-info rounded-3 shadow-sm">
        This quiz has no questions yet.
      </div>

      <div v-else class="accordion shadow-sm rounded-4 overflow-hidden" id="questionsAccordion">
        <div v-for="(question, questionIndex) in quiz.questions" :key="questionIndex"
          class="accordion-item border-0 border-bottom">
          <h2 class="accordion-header" :id="`heading-${questionIndex}`">
            <button class="accordion-button collapsed d-flex align-items-center justify-content-between" type="button"
              data-bs-toggle="collapse" :data-bs-target="`#collapse-${questionIndex}`" aria-expanded="false"
              :aria-controls="`collapse-${questionIndex}`">
              <div class="d-flex align-items-center gap-2 w-100">
                <span class="fw-semibold">
                  {{ questionIndex + 1 }}. {{ question.title }}
                </span>

                <span class="badge rounded-pill ms-auto" :class="question.isOpen ? 'bg-success' : 'bg-secondary'">
                  {{ question.isOpen ? (quiz.currentQuestionIndex == questionIndex ? "Current" : "Open") : "Closed" }}
                </span>
              </div>
            </button>
          </h2>

          <div :id="`collapse-${questionIndex}`" class="accordion-collapse collapse"
            :aria-labelledby="`heading-${questionIndex}`" data-bs-parent="#questionsAccordion">
            <div class="accordion-body bg-light">
              <ul class="list-group list-group-flush rounded-3 overflow-hidden">
                <li v-for="(answer, answerIndex) in question.answers" :key="answerIndex"
                  class="list-group-item d-flex justify-content-between align-items-center px-3 py-3" :class="{
                    'list-group-item-success': answerIndex === question.correctAnswerIndex
                  }">
                  <span>{{ answer }}</span>
                  <span v-if="answerIndex === question.correctAnswerIndex" class="badge bg-success rounded-pill">
                    Correct
                  </span>
                </li>
              </ul>
            </div>
          </div>
        </div>
      </div>
    </div>

    <div v-else class="alert alert-warning rounded-3 shadow-sm">
      Quiz not found.
    </div>
  </div>
</template>