<script setup lang="ts">
import { ref, onMounted } from 'vue';
import { userService } from '../services/userService';
import type { UserResponseDto } from '../Models/responseDtos';
import Quizzes from '../components/Quizzes.vue';

const user = ref<UserResponseDto | null>(null);
const errorMessage = ref('');
const isLoading = ref(false);

onMounted(async () => {
  try {
    user.value = await userService.getUser();
  } catch (error: any) {
    errorMessage.value = error.message;
    console.error(error);
  } finally {
    isLoading.value = false;
  }
});

</script>


<template>
  <div class="container py-4 py-lg-5">
    <div class="row justify-content-center">
      <div class="col-12 col-lg-8 col-xl-7">

        <div class="mb-4">
          <h1 class="fw-bold mb-1">Profile</h1>
          <p class="text-muted mb-0">Your account information and quizzes</p>
        </div>

        <div v-if="isLoading" class="card border-0 shadow-sm rounded-4">
          <div class="card-body p-4 text-center text-muted">
            Loading...
          </div>
        </div>

        <div v-else-if="errorMessage" class="alert alert-danger rounded-3 shadow-sm">
          {{ errorMessage }}
        </div>

        <div v-else-if="user" class="card border-0 shadow-sm rounded-4 mb-4">
          <div class="card-body p-4">
            <h5 class="fw-semibold mb-3">Account Details</h5>

            <div class="row g-3">
              <div class="col-12">
                <div class="d-flex justify-content-between">
                  <span class="text-muted">Id</span>
                  <span class="fw-semibold">{{ user.id }}</span>
                </div>
              </div>

              <div class="col-12">
                <div class="d-flex justify-content-between">
                  <span class="text-muted">Email</span>
                  <span class="fw-semibold">{{ user.email }}</span>
                </div>
              </div>

              <div class="col-12">
                <div class="d-flex justify-content-between">
                  <span class="text-muted">Username</span>
                  <span class="fw-semibold">{{ user.name }}</span>
                </div>
              </div>
            </div>
          </div>
        </div>

        <div class="mb-4">
          <Quizzes />
        </div>

        <div class="d-flex justify-content-end">
          <RouterLink to="/quizzes/create" class="btn btn-success rounded-pill px-4">
            Create Quiz
          </RouterLink>
        </div>

      </div>
    </div>
  </div>
</template>