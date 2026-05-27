<script setup lang="ts">
import { ref } from 'vue'
import { useRouter } from 'vue-router'
import QuizCreator from '../components/QuizCreator.vue'
import { quizService } from '../services/quizService'
import type { QuizRequestDto } from '../Models/requestDtos'

const router = useRouter()

const quiz = ref<QuizRequestDto>({
  title: '',
  questions: [
    {
      title: '',
      answers: ['', ''],
      correctAnswerIndex: 0
    }
  ]
})

const handleCreate = async (data: QuizRequestDto) => {
  await quizService.addQuiz(data)
  router.push('/profile')
}
</script>

<template>
  <div class="container py-4 py-lg-5">
    <div class="row justify-content-center">
      <div class="col-12 col-xl-10">
        <div class="card border-0 shadow-sm rounded-4">
          <div class="card-body p-4 p-lg-5">
            <QuizCreator
              :modelValue="quiz"
              mode="create"
              @submit="handleCreate"
            />
          </div>
        </div>
      </div>
    </div>
  </div>
</template>