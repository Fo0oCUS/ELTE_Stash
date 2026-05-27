<script setup lang="ts">
import { ref } from "vue";
import { quizService } from "../services/quizService";
import { useRouter } from "vue-router";
import { usePlayerStore } from "../stores/playerStore";
import type { JoinRequestDto } from "../Models/requestDtos";

const playerStore = usePlayerStore();
const router = useRouter();

const joinCode = ref("");
const playerName = ref("");
const error = ref("");
const isLoading = ref(false);

const isValidCode = (code: string) => {
  return /^\d{6}$/.test(code);
};

const handleJoin = async () => {
  error.value = "";

  if (!isValidCode(joinCode.value)) {
    error.value = "Join code must be 6 digits";
    return;
  }

  if (!playerName.value) {
    error.value = "Player name is required";
    return;
  }

  isLoading.value = true;

  try {
    const data: JoinRequestDto = {
      joinCode: joinCode.value,
      playerName: playerName.value,
    };

    await quizService.joinQuiz(data);

    playerStore.initialize(data);

    router.push(`/quizzes/play?name=${playerName.value}&code=${joinCode.value}`);
  } catch (e: any) {
    error.value = e.message;
    console.error(e);
  } finally {
    isLoading.value = false;
  }
};
</script>

<template>
  <div class="container py-5">
    <div class="row justify-content-center text-center">
      <div class="col-12 col-md-8 col-lg-6">
        <div class="mb-4">
          <h1 class="fw-bold display-5 mb-2">MWF - Quiz 2026</h1>
          <p class="text-muted fs-5 mb-0">A quiz app for the quizzers!</p>
        </div>

        <div class="card border-0 shadow-sm rounded-4 mx-auto" style="max-width: 420px">
          <div class="card-body p-4 p-lg-5">
            <div class="mb-3 text-start">
              <label class="form-label fw-semibold">Join Code</label>
              <input
                v-model="joinCode"
                class="form-control form-control-lg text-center fw-semibold"
                placeholder="Enter 6 digit code"
                maxlength="6"
              />
            </div>

            <div class="mb-4 text-start">
              <label class="form-label fw-semibold">Player Name</label>
              <input
                v-model="playerName"
                class="form-control form-control-lg"
                placeholder="Your name"
              />
            </div>

            <button class="btn btn-primary btn-lg rounded-pill w-100" @click="handleJoin" :disabled="isLoading">
              {{ isLoading ? "Joining..." : "Join Quiz" }}
            </button>

            <div v-if="error" class="alert alert-danger rounded-3 mt-4 mb-0">
              {{ error }}
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>