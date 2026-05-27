<script setup lang="ts">
import { ref, watch } from 'vue'
import type { QuestionRequestDto, QuizRequestDto } from '../Models/requestDtos'

const props = defineProps<{
  modelValue: QuizRequestDto
  mode: 'create' | 'edit'
}>()

const emit = defineEmits<{
  (e: 'submit', quiz: QuizRequestDto): void
}>()

const errorMessage = ref('')

const quiz = ref<QuizRequestDto>(JSON.parse(JSON.stringify(props.modelValue)))

watch(() => props.modelValue, (newVal) => {
  quiz.value = JSON.parse(JSON.stringify(newVal))
})

const createEmptyQuestion = (): QuestionRequestDto => ({
  title: '',
  answers: ['', ''],
  correctAnswerIndex: 0
})

const addQuestion = () => {
  quiz.value.questions.push(createEmptyQuestion())
}

const removeQuestion = (index: number) => {
  if (quiz.value.questions.length === 1) return
  quiz.value.questions.splice(index, 1)
}

const addAnswer = (qIndex: number) => {
  quiz.value.questions[qIndex].answers.push('')
}

const removeAnswer = (qIndex: number, aIndex: number) => {
  const q = quiz.value.questions[qIndex]

  if (q.answers.length <= 2) return

  q.answers.splice(aIndex, 1)

  if (q.correctAnswerIndex >= q.answers.length) q.correctAnswerIndex = 0
  else if (aIndex < q.correctAnswerIndex) q.correctAnswerIndex--
}

const validate = (): string | null => {
  if (!quiz.value.title.trim()) return 'Quiz title required'

  for (let i = 0; i < quiz.value.questions.length; i++) {
    const q = quiz.value.questions[i]

    if (!q.title.trim()) return `Question ${i + 1} title required`
    if (q.answers.length < 2) return `Question ${i + 1} needs 2 answers`

    for (let a of q.answers) {
      if (!a.trim()) return `Empty answer in question ${i + 1}`
    }
  }

  return null
}

const handleSubmit = () => {
  errorMessage.value = ''

  const err = validate()
  if (err) {
    errorMessage.value = err
    return
  }

  emit('submit', quiz.value)
}
</script>

<template>
  <div class="container py-4 py-lg-5">
    <div class="row justify-content-center">
      <div class="col-12 col-lg-9 col-xl-8">
        <div class="mb-4">
          <h1 class="fw-bold mb-2">{{ mode === 'create' ? 'Create Quiz' : 'Edit Quiz' }}</h1>
          <p class="text-muted mb-0">Add questions, answers, and select the correct option for each question.</p>
        </div>

        <div v-if="errorMessage" class="alert alert-danger rounded-3 shadow-sm">
          {{ errorMessage }}
        </div>

        <form @submit.prevent="handleSubmit">
          <div class="card border-0 shadow-sm rounded-4 mb-4">
            <div class="card-body p-4">
              <label class="form-label fw-semibold">Quiz Title</label>
              <input v-model="quiz.title" class="form-control form-control-lg" placeholder="Quiz title" />
            </div>
          </div>

          <div v-for="(q, qi) in quiz.questions" :key="qi" class="card border-0 shadow-sm rounded-4 mb-4">
            <div class="card-header bg-white border-0 d-flex justify-content-between align-items-center p-4 pb-2">
              <div>
                <strong class="d-block fs-5">Question {{ qi + 1 }}</strong>
                <span class="text-muted small">Write the question and add possible answers.</span>
              </div>

              <button type="button" class="btn btn-outline-danger btn-sm rounded-pill px-3" @click="removeQuestion(qi)">
                Remove
              </button>
            </div>

            <div class="card-body p-4 pt-3">
              <label class="form-label fw-semibold">Question</label>
              <input v-model="q.title" class="form-control mb-4" placeholder="Question" />

              <label class="form-label fw-semibold">Answers</label>

              <div v-for="(a, ai) in q.answers" :key="ai" class="d-flex align-items-center gap-2 mb-2">
                <input v-model="q.answers[ai]" class="form-control" />

                <div class="form-check mb-0">
                  <input
                    type="radio"
                    v-model="q.correctAnswerIndex"
                    :value="ai"
                    class="form-check-input"
                  />
                </div>

                <button
                  class="btn btn-outline-danger btn-sm rounded-circle"
                  type="button"
                  @click="removeAnswer(qi, ai)"
                >
                  -
                </button>
              </div>

              <button
                type="button"
                class="btn btn-outline-primary btn-sm rounded-pill px-3 mt-2"
                @click="addAnswer(qi)"
              >
                + Answer
              </button>
            </div>
          </div>

          <div class="d-flex flex-column flex-sm-row gap-2 justify-content-between mt-4">
            <button type="button" class="btn btn-outline-secondary rounded-pill px-4" @click="addQuestion">
              + Question
            </button>

            <button class="btn btn-primary rounded-pill px-5">
              {{ mode === 'create' ? 'Create' : 'Update' }}
            </button>
          </div>
        </form>
      </div>
    </div>
  </div>
</template>