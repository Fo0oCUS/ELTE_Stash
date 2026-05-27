import { createRouter, createWebHistory } from "vue-router";
import { useUserStore } from "../stores/userStore";
import LoginView from "../views/LoginView.vue";
import HomeView from "../views/HomeView.vue";
import ProfileView from "../views/ProfileView.vue";
import QuizView from "../views/QuizView.vue";
import QuizPlayView from "../views/QuizPlayView.vue";
import RegisterView from "../views/RegisterView.vue";
import EditQuizView from "../views/EditQuizView.vue";
import CreateQuizView from "../views/CreateQuizView.vue";


const router = createRouter({
  history: createWebHistory(),
  routes: [
    {
      path: '/',
      name: 'home',
      component: HomeView
    },
    {
      path: '/login',
      name: 'login',
      component: LoginView
    },
    {
      path: '/profile',
      name: 'profile',
      component: ProfileView,
      meta: { requiresAuth: true }
    },
    {
      path: '/quizzes/create',
      name: 'quiz-create',
      component: CreateQuizView,
      meta: { requiresAuth: true }
    },
    {
      path: '/quizzes/:id/update',
      name: 'quiz-update',
      component: EditQuizView,
      meta: { requiresAuth: true }
    },
    {
      path: '/quizzes/:id',
      name: 'quiz-details',
      component: QuizView,
      meta: { requiresAuth: true }
    },
    {
      path: '/quizzes/play',
      name: 'quiz-play',
      component: QuizPlayView
    },
    {
      path: '/register',
      name: 'register',
      component: RegisterView
    }
  ]
})

router.beforeEach((to) => {
  const userStore = useUserStore()

  userStore.initialize()

  if (userStore.isLoggedIn && userStore.isTokenExpired()) {
    userStore.logout()
  }

  if (to.meta.requiresAuth && !userStore.isLoggedIn) {
    return {
      name: 'login',
      query: { redirect: to.fullPath }
    }
  }

  if (to.name === 'login' && userStore.isLoggedIn) {
    return { name: 'home' }
  }
})

export default router