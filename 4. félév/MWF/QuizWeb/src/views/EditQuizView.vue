<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import QuizCreator from '../components/QuizCreator.vue'
import { quizService } from '../services/quizService'
import type { QuizRequestDto } from '../Models/requestDtos'

const route = useRoute()
const router = useRouter()

const quiz = ref<QuizRequestDto | null>(null)

onMounted(async () => {
  const id = Number(route.params.id)
  const data = await quizService.getQuiz(id)

  quiz.value = {
    title: data.title,
    questions: data.questions.map(q => ({
      title: q.title,
      answers: q.answers,
      correctAnswerIndex: q.correctAnswerIndex
    }))
  }
})

const handleUpdate = async (data: QuizRequestDto) => {
  const id = Number(route.params.id)
  await quizService.updateQuiz(id, data)
  router.push('/profile')
}
</script>

<template>
  <div v-if="quiz" class="container py-4 py-lg-5">
    <div class="row justify-content-center">
      <div class="col-12 col-xl-10">
        <div class="card border-0 shadow-sm rounded-4">
          <div class="card-body p-4 p-lg-5">
            <QuizCreator
              :modelValue="quiz"
              mode="edit"
              @submit="handleUpdate"
            />
          </div>
        </div>
      </div>
    </div>
  </div>

  <div v-else class="container py-5">
    <div class="d-flex justify-content-center align-items-center text-muted">
      Loading...
    </div>
  </div>
</template>