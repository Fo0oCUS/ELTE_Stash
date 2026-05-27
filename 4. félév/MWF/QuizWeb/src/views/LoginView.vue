<script setup lang="ts">
import { ref } from 'vue';
import { userService } from '../services/userService';
import { useRouter } from 'vue-router';
import { useUserStore } from '../stores/userStore';

const userStore = useUserStore();
const router = useRouter();
const email = ref('')
const password = ref('')
const errorMessage = ref('')
const isLoading = ref(false)

const handleLogin = async () => {
    isLoading.value = true
    errorMessage.value = ""
    try{
        const response = await userService.login({
            email: email.value,
            password: password.value
        })
        userStore.login(response)
        router.push("/profile")
    }
    catch(error: any){
        errorMessage.value = error.message;
    }
    finally{
        isLoading.value = false
    }
}


</script>


<template>
  <div class="container py-5">
    <div class="row justify-content-center">
      <div class="col-12 col-sm-10 col-md-6 col-lg-5 col-xl-4">
        <div class="card border-0 shadow-sm rounded-4">
          <div class="card-body p-4 p-lg-5">
            <div class="text-center mb-4">
              <h1 class="fw-bold mb-1">Login</h1>
              <p class="text-muted mb-0">Access your account</p>
            </div>

            <form @submit.prevent="handleLogin">
              <div class="mb-3">
                <label for="email" class="form-label fw-semibold">Email</label>
                <input
                  type="text"
                  v-model="email"
                  id="email"
                  name="email"
                  class="form-control form-control-lg"
                  placeholder="Enter your email"
                >
              </div>

              <div class="mb-4">
                <label for="password" class="form-label fw-semibold">Password</label>
                <input
                  type="password"
                  v-model="password"
                  id="password"
                  name="password"
                  class="form-control form-control-lg"
                  placeholder="Enter your password"
                >
              </div>

              <div class="d-grid">
                <button type="submit" class="btn btn-primary btn-lg rounded-pill" :disabled="isLoading">
                  {{ isLoading ? "Logging in..." : "Login" }}
                </button>
              </div>
            </form>

            <div v-if="errorMessage" class="alert alert-danger mt-4 text-center rounded-3">
              {{ errorMessage }}
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>