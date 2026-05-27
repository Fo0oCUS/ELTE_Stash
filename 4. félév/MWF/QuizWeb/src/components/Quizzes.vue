<script setup lang="ts">
import { QuizStatus } from '../Models/enums'
import type { QuizResponseDto } from '../Models/responseDtos'
import { quizService } from '../services/quizService'
import { ref, onMounted } from 'vue'
import { useRouter } from 'vue-router'

const router = useRouter();
const quizzes = ref<QuizResponseDto[]>([])
const isLoading = ref(true)
const errorMessage = ref('')

onMounted(async () => {
  try {
    quizzes.value = await quizService.getUserQuizzes()
  } catch (error: any) {
    errorMessage.value = error.message;
    console.error(error)
  } finally {
    isLoading.value = false
  }
})

const getStatusClass = (status: QuizStatus) => {
  switch (status) {
    case QuizStatus.waitingToBePublished:
      return 'bg-secondary'
    case QuizStatus.inProgress:
      return 'bg-warning text-dark'
    case QuizStatus.finished:
      return 'bg-success'
    default:
      return 'bg-light text-dark'
  }
}

const handleOpenQuiz = (id: number) => {
  router.push({
    name: "quiz-details",
    params: { id }
  });
}
</script>

<template>
  <div class="container py-4 py-lg-5">
    <div class="d-flex flex-column flex-sm-row justify-content-between align-items-sm-center gap-3 mb-4">
      <div>
        <h2 class="fw-bold mb-1">My Quizzes</h2>
        <p class="text-muted mb-0">Manage, edit, and open your created quizzes.</p>
      </div>
    </div>

    <div v-if="isLoading" class="card border-0 shadow-sm rounded-4">
      <div class="card-body p-4 text-center text-muted">
        Loading...
      </div>
    </div>

    <div v-else-if="errorMessage" class="alert alert-danger rounded-3 shadow-sm">
      {{ errorMessage }}
    </div>

    <div v-else-if="quizzes.length === 0" class="card border-0 shadow-sm rounded-4">
      <div class="card-body p-5 text-center">
        <h5 class="fw-semibold mb-2">No quizzes yet</h5>
        <p class="text-muted mb-0">Create your first quiz to see it listed here.</p>
      </div>
    </div>

    <div v-else class="row g-4">
      <div v-for="quiz in quizzes" :key="quiz.id" class="col-lg-6">
        <div class="card h-100 border-0 shadow-sm rounded-4 overflow-hidden">
          <div class="card-body p-4">
            <div class="d-flex justify-content-between align-items-start gap-3 mb-3">
              <h5 class="card-title fw-bold mb-0">{{ quiz.title }}</h5>

              <span class="badge rounded-pill" :class="getStatusClass(quiz.status)">
                {{ quiz.status }}
              </span>
            </div>

            <p class="card-text text-muted mb-0">
              Questions: <span class="fw-semibold text-dark">{{ quiz.questions.length }}</span>
            </p>
          </div>

          <div class="card-footer bg-white border-0 p-4 pt-0">
            <div class="d-flex justify-content-between align-items-center gap-2 mb-3">
              <small class="text-muted">
                {{ quiz.isPublished ? 'Published' : 'Draft' }}
              </small>
            </div>

            <div class="d-flex gap-2">
              <RouterLink
                :to="{ name: 'quiz-update', params: { id: quiz.id } }"
                class="btn btn-sm btn-outline-warning rounded-pill px-3 flex-fill"
                :class="{ 
                  'disabled': quiz.isPublished 
                }"
              >
                Edit
              </RouterLink>

              <button class="btn btn-sm btn-primary rounded-pill px-3 flex-fill" @click="handleOpenQuiz(quiz.id)">
                Open
              </button>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>